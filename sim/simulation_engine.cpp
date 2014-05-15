#include "sim/simulation_engine.h"

#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>

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

    Module mod;
    mod.this_in = llvm::PTOGV(root_ptr);
    mod.this_out = llvm::PTOGV(root_ptr);
    
    for(auto proc : m_top_mod->processes) {
      Process p;
      p.function = m_code->get_process(proc);
      mod.processes.push_back(p);
    }

    m_modules.push_back(mod);
  }


  bool 
  Simulation_engine::simulate_cycle() {
    std::cout << "----- simulate cycle -----\n";
    //  // run all processes initially
    //  // TODO generate allocation functions for output module in simulation frame
    //  // TODO data structure for process sensitivity list and read registration
    //  // TODO run list data structure (C++) and delta cycle simulation
    //  // TODO support for periodic processes

    for(auto const& mod : m_modules) {
      std::vector<llvm::GenericValue> args{
        mod.this_in, mod.this_out 
      };

      for(auto const& proc : mod.processes) {
        m_exe->runFunction(proc.function, args);
      }
    }

    //cout << "top:\n" << hex
      //<< "0x" << setw(16) << setfill('0')
      //<< static_cast<uint64_t*>(root_ptr)[0]
      //<< " "
      //<< "0x" << setw(16) << setfill('0')
      //<< static_cast<uint64_t*>(root_ptr)[1]
      //<< endl;


    return true;
  }


  void
  Simulation_engine::teardown() {
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

    EngineBuilder exe_bld(m_code->module().get());
    std::string err_str;
    exe_bld.setErrorStr(&err_str);
    exe_bld.setEngineKind(EngineKind::JIT);
    m_exe = exe_bld.create();
    m_exe->DisableSymbolSearching(false);
    if( !m_exe ) {
      cerr << "Failed to create execution engine!" << endl;
      cerr << err_str << endl;
    }
  }

}
