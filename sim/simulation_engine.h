#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
#include <map>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>

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
      ~Simulation_engine();


      //
      // public member functions
      //

      void setup();
      void simulate(ir::Time const& duration);
      void teardown();

      Module_inspector inspect_module(ir::Label const& name);

    protected:
      struct Process {
        llvm::Function* function;
        void* exe_ptr;
        bool sensitive = true;

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
      typedef std::unordered_set<Process, Process_hash> Process_set;
      typedef std::multimap<ir::Time, Process> Time_process_map;
      typedef std::multimap<ir::Time, std::tuple<ir::Time,Process>> Process_schedule;

      struct Module {
        void* this_in;
        void* this_out;
        char* read_mask;
        std::size_t read_mask_sz;
        llvm::StructLayout const* layout;
        Process_list processes;
        Time_process_map periodicals;
        unsigned int num_elements;
        std::vector<Process_set> sensitivity;
        Process_set run_list;
        Process_schedule schedule;
      };

      typedef std::vector<Module> Module_list;


      static unsigned const max_cycles = 20;


      llvm::ExecutionEngine* m_exe = nullptr;
      llvm::DataLayout const* m_layout = nullptr;
      std::shared_ptr<sim::Llvm_library> m_lib;
      std::shared_ptr<Llvm_module> m_top_mod;
      Module_list m_modules;
      ir::Time m_time;
      bool m_setup_complete = false;


      void init(std::string const& filename, std::string const& toplevel);
      ir::Time simulate_step(ir::Time const& t, ir::Time const& duration);
      bool simulate_cycle();


    public:
      Llvm_impl::Module::Frame allocate_module_frame(std::shared_ptr<Llvm_module> mod);
      Llvm_impl::Module::Read_mask allocate_read_mask(std::shared_ptr<Llvm_module> mod);
  };



  class Instrumented_simulation_engine : public Simulation_engine {
    public:
      Instrumented_simulation_engine(std::string const& filename,
          std::string const& toplevel)
        : Simulation_engine(filename, toplevel) {
      }

      void simulate(ir::Time const& duration);

      void instrument(Instrumenter_if& instr) { m_instrumenter = &instr; }


    private:
      Instrumenter_if* m_instrumenter = nullptr;
  };
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
