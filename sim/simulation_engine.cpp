#include "sim/simulation_engine.h"

#include <iomanip>
#include <algorithm>
#include <iterator>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Analysis/Verifier.h>

#include "parse_driver.h"
#include "sim/compile.h"
#include "ir/find_hierarchy.h"
#include "ir/builtins.h"
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
  Simulation_engine::setup() {
    using namespace std;

    cout << "setup for simulation..." << endl;

    // add mappings for runtime functions
    m_exe->addGlobalMapping(m_code->get_function(ir::Builtins::functions.at("print")),
        (void*)(&print));

    // generate wrapper function to setup simulation
    m_code->create_setup(m_top_mod);
    m_code->emit();

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

      mod.run_list.insert(p);

      auto pr = std::make_pair(proc.period, p);
      mod.periodicals.insert(pr);
      mod.schedule.insert(pr);
    }

    m_modules.push_back(mod);
  }


  void
  Simulation_engine::simulate(ir::Time const& duration) {
    for(ir::Time t=m_time; t<(m_time + duration); ) {
      // add timed processes to the run list
      for(auto& mod : m_modules) {
        auto timed_procs_range = mod.schedule.equal_range(t);
        for(auto it=timed_procs_range.first;
            it != timed_procs_range.second;
            ++it) {
          mod.run_list.insert(it->second);
        }

        mod.schedule.erase(timed_procs_range.first, timed_procs_range.second);
      }

      // simulate cycles until all signals are stable
      unsigned int cycle = 0;
      bool modified;

      do {
        modified = simulate_cycle();
      } while( (cycle++ < max_cycles) && (modified) );


      // select next point in time for simulation
      t = t + duration;
    }
  }


  void
  Simulation_engine::teardown() {
    for(auto& mod : m_modules) {
      delete [] mod.read_mask;
    }
    m_modules.clear();
  }


  void
  Simulation_engine::init(std::string const& filename, std::string const& toplevel) {
    using namespace llvm;
    using namespace std;

    Parse_driver driver;
    if( driver.parse(filename) )
      throw std::runtime_error("parse failed");

    std::tie(m_top_ns, m_code) = sim::compile(driver.ast_root());

    m_top_mod = find_by_path(m_top_ns, &ir::Namespace::modules, toplevel);
    if( !m_top_mod ) {
      cerr << "Can not find top level module '"
        << toplevel 
        << "'\n";
      cerr << "The following modules were found in toplevel namespace '"
        << m_top_ns.name 
        << "':\n";
      for(auto m : m_top_ns.modules) {
        cerr << "    " << m.first << '\n';
      }

      return;
    }

    verifyModule(*(m_code->module())); 

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

    m_layout = m_exe->getDataLayout();
  }


  bool 
  Simulation_engine::simulate_cycle() {
    using namespace std;

    cout << "----- simulate cycle -----\n";

    for(auto& mod : m_modules) {
      // clear sensitivity list
      for(auto s : mod.sensitivity)
        s.clear();

      cout << "running " << mod.run_list.size() << " processes" << endl;

      for(auto const& proc : mod.run_list) {
        cout << "calling process..." << endl;
        std::fill_n(mod.read_mask, mod.read_mask_sz, 0);
        auto exe_ptr = reinterpret_cast<void (*)(void*, void*, void*)>(proc.exe_ptr);
        exe_ptr(mod.this_out, mod.this_in, mod.read_mask);

        cout << "read_mask: " << hex;
        for(size_t j=0; j<mod.read_mask_sz; j++)
          cout << setw(2) << setfill('0') << static_cast<int>(mod.read_mask[j]) << " ";
        cout << endl; 

        // add to sensitivity list
        for(size_t j=0; j<mod.read_mask_sz; j++) {
          if( mod.read_mask[j] )
            mod.sensitivity[j].push_back(proc);
        }
      }
    }

    // find modified signals
    bool modified = false;
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
          modified = true;
          mod_modified = true;

          // add dependant processes to run list
          for(auto const& dep : mod.sensitivity[elem]) {
            mod.run_list.insert(dep);
          }
        }
      }

      if( mod_modified )
        copy(ptr_out, ptr_out + size, ptr_in);
    }


    auto root_ptr = m_exe->getPointerToGlobal(m_code->root());
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
      << endl;

    return modified;
  }

}
