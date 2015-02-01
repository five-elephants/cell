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

    // TODO
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

    /*verifyModule(*(m_code->module())); 

    EngineBuilder exe_bld(m_code->module().get());
    std::string err_str;
    exe_bld.setErrorStr(&err_str);
    exe_bld.setEngineKind(EngineKind::JIT);
    m_exe = exe_bld.create();
    m_exe->DisableSymbolSearching(false);
    if( !m_exe ) {
      std::stringstream strm;
      strm << "Failed to create execution engine!: " << err_str;
      throw std::runtime_error(strm.str());
    }

    m_layout = m_exe->getDataLayout();*/
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


    Module mod;
    mod.this_in = root_ptr;
    mod.this_out = root_b_ptr;
    mod.layout = m_layout->getStructLayout(m_code->get_module_type(m_top_mod.get()));
    mod.num_elements = m_code->get_module_type(m_top_mod.get())->getNumElements();
    mod.sensitivity.resize(mod.num_elements);
    mod.read_mask_sz = m_layout->getTypeAllocSize(
      llvm::ArrayType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 1),
          mod.num_elements) 
    );
    mod.read_mask = new char [mod.read_mask_sz];
    
    for(auto proc : m_top_mod->processes) {
      Process p;
      p.function = m_code->get_process(proc);
      p.exe_ptr = m_exe->getPointerToFunction(p.function);

      mod.processes.push_back(p);
      mod.run_list.insert(p);
    }

    for(auto proc : m_top_mod->periodicals) {
      Process p;
      p.function = m_code->get_process(proc);
      p.exe_ptr = m_exe->getPointerToFunction(p.function);
      p.sensitive = false;

      mod.run_list.insert(p);

      mod.periodicals.insert(std::make_pair(proc->period, p));
      mod.schedule.insert(std::make_pair(proc->period, std::make_tuple(proc->period, p)));
    }

    m_modules.push_back(mod);
    m_setup_complete = true;
*/
  }


  void
  Simulation_engine::simulate(ir::Time const& duration) {
    for(ir::Time t=m_time; t<(m_time + duration); ) {
      t = simulate_step(t, duration);
    }
  }


  void
  Simulation_engine::teardown() {
    for(auto& mod : m_modules) {
      delete [] mod.read_mask;
    }
    m_modules.clear();

    m_setup_complete = false;
  }


  Module_inspector
  Simulation_engine::inspect_module(ir::Label const& name) {
    if( !m_setup_complete )
      throw std::runtime_error("Call Simulation_engine::setup() before Simulation_engine::inspect_module()");

    // TODO lookup module in hierarchy
    /*auto root_ptr = m_exe->getPointerToGlobal(m_code->root());
    auto layout = m_layout->getStructLayout(m_code->get_module_type(m_top_mod.get()));
    auto num_elements = m_code->get_module_type(m_top_mod.get())->getNumElements();

    Module_inspector rv(m_top_mod, root_ptr, layout, num_elements);

    return rv;*/
  }


  ir::Time
  Simulation_engine::simulate_step(ir::Time const& t, ir::Time const& duration) {
    ir::Time next_t = m_time + duration;

    std::cout << "===== time: " << t << " =====" << std::endl;

    // add timed processes to the run list
    for(auto& mod : m_modules) {
      std::list<Process_schedule::value_type> new_schedules;

      auto timed_procs_range = mod.schedule.equal_range(t);
      for(auto it=timed_procs_range.first;
          it != timed_procs_range.second;
          ++it) {
        auto period = std::get<0>(it->second);
        auto proc = std::get<1>(it->second);
        mod.run_list.insert(proc);
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

    return next_t;
  }

  bool 
  Simulation_engine::simulate_cycle() {
    using namespace std;

    cout << "----- simulate cycle -----\n";

    for(auto& mod : m_modules) {
      cout << "running " << mod.run_list.size() << " processes" << endl;

      for(auto const& proc : mod.run_list) {
        cout << "calling process..." << endl;
        if( proc.sensitive )
          std::fill_n(mod.read_mask, mod.read_mask_sz, 0);
        auto exe_ptr = reinterpret_cast<void (*)(void*, void*, void*)>(proc.exe_ptr);
        exe_ptr(mod.this_out, mod.this_in, mod.read_mask);

        if( proc.sensitive ) {
          cout << "read_mask: " << hex;
          for(size_t j=0; j<mod.read_mask_sz; j++)
            cout << setw(2) << setfill('0') << static_cast<int>(mod.read_mask[j]) << " ";
          cout << endl; 

          // add to sensitivity list
          for(size_t j=0; j<mod.read_mask_sz; j++) {
            if( mod.read_mask[j] )
              mod.sensitivity[j].insert(proc);
            else
              mod.sensitivity[j].erase(proc); 
          }
        }
      }
    }

    // find modified signals
    bool rerun = false;
    for(auto& mod : m_modules) {
      char* ptr_in = static_cast<char*>(mod.this_in);
      char* ptr_out = static_cast<char*>(mod.this_out);
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





  void
  Instrumented_simulation_engine::simulate(ir::Time const& duration) {
    /*for(ir::Time t=m_time; t<(m_time + duration); ) {
      t = simulate_step(t, duration);

      if( m_instrumenter ) {
        for(auto const& mod : m_modules) {
          Module_inspector insp(m_top_mod, mod.this_in, mod.layout, mod.num_elements);
          m_instrumenter->module(t, insp);
        }
      }
    }*/
  }
}
