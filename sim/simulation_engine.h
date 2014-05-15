#pragma once

#include <string>
#include <memory>
#include <vector>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include "sim/llvm_codegen.h"
#include "ir/namespace.h"

namespace sim {

  class Simulation_engine {
    public:
      Simulation_engine(std::string const& filename,
          std::string const& toplevel);

      void setup();
      bool simulate_cycle();
      void teardown();


    private:
      struct Process {
        llvm::Function* function;
      };

      typedef std::vector<Process> Process_list;

      struct Module {
        llvm::GenericValue this_in;
        llvm::GenericValue this_out;
        llvm::Function* allocator;
        Process_list processes;
      };

      typedef std::vector<Module> Module_list;



      llvm::ExecutionEngine* m_exe = nullptr;
      std::shared_ptr<sim::Llvm_codegen> m_code;
      ir::Namespace m_top_ns;
      std::shared_ptr<ir::Module> m_top_mod;
      Module_list m_modules;

      void init(std::string const& filename, std::string const& toplevel);
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
