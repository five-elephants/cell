#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
#include <map>
#include <stdexcept>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/PassManager.h>
#include <log4cxx/logger.h>

#include "sim/runset.h"
#include "sim/module_inspector.h"
#include "sim/instrumenter_if.h"
#include "sim/llvm_namespace.h"
#include "ir/find_hierarchy.h"
#include "ir/time.h"


namespace sim {

  class Simulation_engine {
    public:
      //
      // constructors
      //

      Simulation_engine(std::string const& filename,
          std::string const& toplevel);
      Simulation_engine(std::string const& filename,
          std::string const& toplevel,
          std::vector<std::string> const& lookup_path);
      Simulation_engine(std::string const& filename);
      Simulation_engine(std::string const& filename,
          std::vector<std::string> const& lookup_path);
      ~Simulation_engine();


      //
      // public member functions
      //

      void setup();
      void setup(std::string const& toplevel) {
        set_toplevel(toplevel);
        setup();
      }
      void simulate(ir::Time const& duration);
      void teardown();


      /** Create a Module_inspector object
       *
       * @param name Path to a module instance within the design
       *
       * Allows for inspection and modification of values of a module instance
       * in the design. The path is searched using ir::find_instance().
       * */
      Module_inspector inspect_module(ir::Label const& name);


      /** Add a driver/observer callback
       *
       * @tparam Callable Type of callback
       * @param driver Callback to be used as driver/observer
       * @param path Path to the module to drive/observe
       * @return Iterator to the callback
       *
       * This method registers a callback that gets called in every simulation
       * cycle. The callback takes four arguments:
       *
       *  - Current simulation time as ir::Time object.
       *  - Runset::Module_frame object for this_in.
       *  - Runset::Module_frame object for this_out.
       *  - Runset::Module_frame object for this_prev.
       *
       * The this_in, this_out, and this_prev pointers can be manipulated
       * freely by the callback. This allows to implement monitors or drivers.
       * */
      template<typename Callable>
      Runset::Driver_list::iterator
      add_driver(Callable& driver, ir::Label const& path) {
        if( !m_setup_complete )
          throw std::runtime_error("Call Simulation_engine::setup() before "
              "Simulation_engine::add_driver()");

        // find module in hierarchy
        auto mod = ir::find_instance(m_top_mod, path);
        if( !mod )
          throw std::runtime_error("Could not find requested module");

        // find module in runset
        auto it = std::find_if(std::begin(m_runset.modules),
            std::end(m_runset.modules),
            [&mod](Runset::Module m) -> bool { return m.mod == mod; });
        if( it == std::end(m_runset.modules) ) {
          throw std::runtime_error("Module not available in runset");
        }

        // register driver callback
        it->drivers.push_back(std::ref(driver));
        return --std::end(it->drivers);
      }


      std::shared_ptr<sim::Llvm_library> library() { return m_lib; }


    protected:
      static unsigned const max_cycles = 20;


      Runset m_runset;
      llvm::ExecutionEngine* m_exe = nullptr;
      llvm::DataLayout const* m_layout = nullptr;
      std::shared_ptr<sim::Llvm_library> m_lib;
      std::shared_ptr<Llvm_module> m_top_mod;
      ir::Time m_time;
      bool m_setup_complete = false;
      log4cxx::LoggerPtr m_logger;
      std::shared_ptr<llvm::PassManager> m_mpm;
      std::shared_ptr<llvm::FunctionPassManager> m_fpm;


      void init(std::string const& filename,
          std::vector<std::string> const& lookup_path);
      void optimize();
      void set_toplevel(std::string const& toplevel);
      ir::Time simulate_step(ir::Time const& t, ir::Time const& duration);
      bool simulate_cycle(ir::Time const& t);
  };



  class Instrumented_simulation_engine : public Simulation_engine {
    public:
      Instrumented_simulation_engine(std::string const& filename,
          std::string const& toplevel)
        : Simulation_engine(filename, toplevel) {
      }

      Instrumented_simulation_engine(std::string const& filename,
          std::string const& toplevel,
          std::vector<std::string> const& lookup_path)
        : Simulation_engine(filename, toplevel, lookup_path) {
      }

      void setup();
      void setup_module(std::shared_ptr<Llvm_module> mod);
      void simulate(ir::Time const& duration);

      void instrument(Instrumenter_if& instr) { m_instrumenter = &instr; }


    private:
      Instrumenter_if* m_instrumenter = nullptr;
  };
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
