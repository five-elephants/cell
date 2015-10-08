#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
#include <map>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/PassManager.h>
#include <log4cxx/logger.h>

#include "sim/runset.h"
#include "sim/module_inspector.h"
#include "sim/instrumenter_if.h"
#include "sim/llvm_namespace.h"
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
      ~Simulation_engine();


      //
      // public member functions
      //

      void setup();
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
          std::string const& toplevel,
          std::vector<std::string> const& lookup_path);
      void optimize();
      ir::Time simulate_step(ir::Time const& t, ir::Time const& duration);
      bool simulate_cycle();
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
