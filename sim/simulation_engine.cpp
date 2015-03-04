#include "sim/simulation_engine.h"

#include <iomanip>
#include <algorithm>
#include <iterator>
#include <list>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Analysis/Verifier.h>

#include "parse_driver.h"
#include "sim/llvm_namespace_scanner.h"
#include "ast/ast_printer.h"
#include "sim/llvm_builtins.h"
#include "ir/find_hierarchy.h"
#include "sim/runtime.h"

namespace sim {



  Simulation_engine::Simulation_engine(std::string const& filename,
      std::string const& toplevel) {
    init(filename, toplevel);
  }


  Simulation_engine::~Simulation_engine() {
    teardown();
  }


  void
  Simulation_engine::init(std::string const& filename, std::string const& toplevel) {
    using namespace llvm;
    using namespace std;

    Parse_driver driver;
    if( driver.parse(filename) )
      throw std::runtime_error("parse failed");

    m_lib = std::make_shared<ir::Library<sim::Llvm_impl>>();

    m_lib->name = "main";
    m_lib->ns = std::make_shared<sim::Llvm_namespace>();
    m_lib->ns->enclosing_library = m_lib;
    m_lib->impl = sim::create_library_impl(m_lib->name);

    // LLVM initialization
    llvm::InitializeNativeTarget();

    // init builtins
    init_builtins(m_lib);

    // print AST
    ast::Ast_printer printer(std::cout);
    driver.ast_root().accept(printer);

    // generate code
    sim::Llvm_namespace_scanner scanner(*(m_lib->ns));
    driver.ast_root().accept(scanner);

    // show generated code
    cout << "Generated code:\n=====\n";
    m_lib->impl.module->dump();
    cout << "\n====="
      << endl;

    verifyModule(*(m_lib->impl.module));

    // create JIT execution engine
    EngineBuilder exe_bld(m_lib->impl.module.get());
    string err_str;
    exe_bld.setErrorStr(&err_str);
    exe_bld.setEngineKind(EngineKind::JIT);

    m_exe = exe_bld.create();
    if( !m_exe ) {
      stringstream strm;
      strm << "Failed to create execution engine!: " << err_str;
      throw std::runtime_error(strm.str());
    }
    // no lookup using dlsym
    m_exe->DisableSymbolSearching(true);

    m_layout = m_exe->getDataLayout();
    m_runset.layout(m_layout);

    m_top_mod = find_by_path(*(m_lib->ns), &ir::Namespace<Llvm_impl>::modules, toplevel);
    if( !m_top_mod ) {
      cerr << "Can not find top level module '"
        << toplevel
        << "'\n";
      cerr << "The following modules were found in toplevel namespace '"
        << m_lib->ns->name
        << "':\n";
      for(auto m : m_lib->ns->modules) {
        cerr << "    " << m.first << '\n';
      }

      return;
    }
  }


  void
  Simulation_engine::setup() {
    using namespace std;

    cout << "setup for simulation..." << endl;

/*
    // add mappings for runtime functions
    m_exe->addGlobalMapping(m_code->get_function(ir::Builtins::functions.at("print")),
        (void*)(&print));

    // generate wrapper function to setup simulation
    m_code->create_setup(m_top_mod);
    //m_code->emit();

    auto setup_func = m_code->module()->getFunction("setup");
    if( !setup_func ) {
      throw std::runtime_error("failed to find function setup()");
    }

    void* ptr = m_exe->getPointerToFunction(setup_func);
    void(*func)() = (void(*)())(ptr);
    func();

    // Init process list
    // TODO find all processes in hierarchy
    auto root_ptr = m_exe->getPointerToGlobal(m_code->root());
    void* root_b_ptr = static_cast<void*>(static_cast<char*>(root_ptr)
        + m_layout->getTypeAllocSize(m_code->get_module_type(m_top_mod.get())));
*/

    m_runset.add_module(m_exe, m_top_mod);

    m_setup_complete = true;
  }


  void
  Simulation_engine::simulate(ir::Time const& duration) {
    for(ir::Time t=m_time; t<(m_time + duration); ) {
      t = simulate_step(t, duration);
    }
  }


  void
  Simulation_engine::teardown() {
    m_setup_complete = false;
  }


  Module_inspector
  Simulation_engine::inspect_module(ir::Label const& name) {
    if( !m_setup_complete )
      throw std::runtime_error("Call Simulation_engine::setup() before Simulation_engine::inspect_module()");

    // TODO search the module in instances under the top module
    auto& mod = m_top_mod;
    //std::shared_ptr<sim::Llvm_module> mod = ir::find_by_path<sim::Llvm_module>(*(m_lib->ns),
        //&ir::Namespace<sim::Llvm_impl>::modules,
        //"m");
    //if( !mod )
      //throw std::runtime_error("failed to find module in design hierarchy");

    auto layout = m_layout->getStructLayout(mod->impl.mod_type);
    auto num_elements = mod->impl.mod_type->getNumElements();

    Module_inspector rv(mod,
        layout,
        num_elements,
        m_exe,
        m_runset);

    return rv;
  }


  ir::Time
  Simulation_engine::simulate_step(ir::Time const& t, ir::Time const& duration) {
    ir::Time next_t = m_time + duration;

    std::cout << "===== time: " << t << " =====" << std::endl;

    // add timed processes to the run list
    for(auto& mod : m_runset.modules) {
      std::list<Runset::Process_schedule::value_type> new_schedules;

      auto timed_procs_range = mod.schedule.equal_range(t);
      for(auto it=timed_procs_range.first;
          it != timed_procs_range.second;
          ++it) {
        auto period = std::get<0>(it->second);
        auto proc = std::get<1>(it->second);
        mod.run_list.insert(proc);

        if( period.v > 0 )
          new_schedules.push_back(std::make_pair(t + period, it->second));
      }

      mod.schedule.erase(timed_procs_range.first, timed_procs_range.second);
      std::move(new_schedules.begin(),
          new_schedules.end(),
          std::inserter(mod.schedule, mod.schedule.begin()));

      // select next point in time for simulation
      auto nextit = mod.schedule.upper_bound(m_time);
      if( nextit != mod.schedule.end() )
        next_t = std::min(next_t, nextit->first);
    }

    // simulate cycles until all signals are stable
    unsigned int cycle = 0;
    bool rerun;

    do {
      rerun = simulate_cycle();
    } while( (cycle++ < max_cycles) && rerun );

    // update this_prev module frame
    //for(auto& mod : m_runset.modules) {
      //std::copy(mod.this_out->begin(), mod.this_out->end(), mod.this_prev->begin());
    //}

    return next_t;
  }

  bool
  Simulation_engine::simulate_cycle() {
    using namespace std;

    cout << "----- simulate cycle -----\n";

    for(auto& mod : m_runset.modules) {
      cout << "running " << mod.run_list.size() << " processes" << endl;

      for(auto const& proc : mod.run_list) {
        cout << "calling process..." << endl;
        if( proc.sensitive )
          std::fill(mod.read_mask->begin(), mod.read_mask->end(), 0);
        auto exe_ptr = reinterpret_cast<void (*)(char*, char*, char*,char*)>(proc.exe_ptr);
        exe_ptr(mod.this_out->data(),
            mod.this_in->data(),
            mod.this_prev->data(),
            mod.read_mask->data());

        if( proc.sensitive ) {
          cout << "read_mask: " << hex;
          for(size_t j=0; j<mod.read_mask->size(); j++)
            cout << setw(2) << setfill('0')
              << static_cast<int>((*(mod.read_mask))[j]) << " ";
          cout << endl;

          // add to sensitivity list
          for(size_t j=0; j<mod.read_mask->size(); j++) {
            if( (*(mod.read_mask))[j] )
              mod.sensitivity[j].insert(proc);
            else
              mod.sensitivity[j].erase(proc);
          }
        }
      }
    }

    // find modified signals
    bool rerun = false;
    for(auto& mod : m_runset.modules) {
      char* ptr_in = mod.this_in->data();
      char* ptr_out = mod.this_out->data();
      auto size = mod.layout->getSizeInBytes();
      mod.run_list.clear();

      bool mod_modified = false;
      for(size_t i=0; i<size; i++) {
        if( ptr_out[i] != ptr_in[i] ) {
          cout << "found mismatch at offset " << i;
          auto elem = mod.layout->getElementContainingOffset(i);
          cout << " belongs to element " << elem << endl;
          mod_modified = true;

          // add dependant processes to run list
          for(auto const& dep : mod.sensitivity[elem]) {
            mod.run_list.insert(dep);
          }
        }
      }

      // safe this_in to this_prev frame
      std::copy(mod.this_in->begin(),
          mod.this_in->end(),
          mod.this_prev->begin());

      if( mod_modified )
        copy(ptr_out, ptr_out + size, ptr_in);

      if( !mod.run_list.empty() )
        rerun = true;

    }


    /*auto root_ptr = m_exe->getPointerToGlobal(m_code->root());
    cout << "top A:\n" << hex
      << "0x" << setw(16) << setfill('0')
      << static_cast<uint64_t*>(root_ptr)[0]
      << " "
      << "0x" << setw(16) << setfill('0')
      << static_cast<uint64_t*>(root_ptr)[1]
      << endl;

    auto off = m_layout->getTypeAllocSize(m_code->get_module_type(m_top_mod.get()));
    cout << "top B:\n" << hex
      << "0x" << setw(16) << setfill('0')
      << static_cast<uint64_t*>(root_ptr)[off/sizeof(uint64_t)]
      << " "
      << "0x" << setw(16) << setfill('0')
      << static_cast<uint64_t*>(root_ptr)[1 + off/sizeof(uint64_t)]
      << endl;*/

    return rerun;
  }



  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------


  void
  Instrumented_simulation_engine::setup() {
    Simulation_engine::setup();

    if( m_instrumenter ) {
      for(auto const& mod : m_runset.modules) {
        auto num_elements = mod.mod->impl.mod_type->getNumElements();
        auto layout = m_layout->getStructLayout(mod.mod->impl.mod_type);
        Module_inspector insp(mod.mod,
            layout,
            num_elements,
            m_exe,
            m_runset);
        m_instrumenter->module(ir::Time(0, ir::Time::ns), insp);
      }
    }
  }


  void
  Instrumented_simulation_engine::simulate(ir::Time const& duration) {
    for(ir::Time t=m_time; t<(m_time + duration); ) {
      ir::Time next_t = simulate_step(t, duration);

      if( m_instrumenter ) {
        for(auto const& mod : m_runset.modules) {
          auto num_elements = mod.mod->impl.mod_type->getNumElements();
          auto layout = m_layout->getStructLayout(mod.mod->impl.mod_type);
          Module_inspector insp(mod.mod,
              layout,
              num_elements,
              m_exe,
              m_runset);
          m_instrumenter->module(t, insp);
        }
      }

      t = next_t;
    }
  }

}
