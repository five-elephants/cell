#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
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
        void* exe_ptr;

        bool operator == (Simulation_engine::Process const& b) const {
          return (function == b.function) && (exe_ptr == b.exe_ptr);
        }
      };

      struct Process_hash {
        std::size_t operator () (Simulation_engine::Process const& a) const {
          auto h1 = std::hash<llvm::Function*>()(a.function);
          auto h2 = std::hash<void*>()(a.exe_ptr);
          return h1 ^ (h2 << 1);
        }
      };

      typedef std::vector<Process> Process_list;
      typedef std::unordered_set<Process, Process_hash> Run_list;

      struct Module {
        void* this_in;
        void* this_out;
        llvm::StructLayout const* layout;
        Process_list processes;
        unsigned int num_elements;
        std::vector<Process_list> sensitivity;
        Run_list run_list;
      };

      typedef std::vector<Module> Module_list;


      llvm::ExecutionEngine* m_exe = nullptr;
      llvm::DataLayout const* m_layout = nullptr;
      std::shared_ptr<sim::Llvm_codegen> m_code;
      ir::Namespace m_top_ns;
      std::shared_ptr<ir::Module> m_top_mod;
      Module_list m_modules;


      void init(std::string const& filename, std::string const& toplevel);
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
