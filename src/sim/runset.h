#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <map>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "sim/llvm_namespace.h"
#include "ir/time.h"


namespace sim {

  class Runset {
    public:

      //
      // member types
      //

      struct Process {
        llvm::Function* function;
        void* exe_ptr;
        bool sensitive = true;

        bool operator == (Runset::Process const& b) const {
          return (function == b.function) && (exe_ptr == b.exe_ptr);
        }
      };

      struct Process_hash {
        std::size_t operator () (Runset::Process const& a) const {
          auto h1 = std::hash<llvm::Function*>()(a.function);
          auto h2 = std::hash<void*>()(a.exe_ptr);
          return h1 ^ (h2 << 1);
        }
      };

      typedef std::vector<Process> Process_list;
      typedef std::unordered_set<Process, Process_hash> Process_set;
      typedef std::multimap<ir::Time, Process> Time_process_map;
      typedef std::multimap<ir::Time, std::tuple<ir::Time,Process>> Process_schedule;
      typedef std::shared_ptr<std::vector<char>> Module_frame;
      typedef std::shared_ptr<std::vector<char>> Read_mask;


      struct Module {
        std::shared_ptr<Llvm_module> mod;
        Module_frame this_in;
        Module_frame this_out;
        Module_frame this_prev;
        Read_mask read_mask;
        llvm::StructLayout const* layout = nullptr;
        Process_list processes;
        Time_process_map periodicals;
        std::vector<Process_set> sensitivity;
        Process_set run_list;
        Process_schedule schedule;
        Time_process_map recurrent_schedule;
      };

      typedef std::vector<Module> Module_list;


      //
      // member functions
      //

      void add_module(llvm::ExecutionEngine* exe,
          std::shared_ptr<Llvm_module> mod);

      void setup_hierarchy();
      void call_init(llvm::ExecutionEngine* exe);


      Module_frame allocate_module_frame(std::shared_ptr<Llvm_module> mod);
      Read_mask allocate_read_mask(std::shared_ptr<Llvm_module> mod);

      std::size_t module_frame_size(std::shared_ptr<Llvm_module> mod) const {
        return m_layout->getTypeAllocSize(mod->impl.mod_type);
      }

      std::size_t read_mask_size(std::shared_ptr<Llvm_module> mod) const {
        auto read_mask_ty = llvm::ArrayType::get(
            llvm::IntegerType::get(llvm::getGlobalContext(), 1),
            mod->impl.mod_type->getNumElements()
          );
        return m_layout->getTypeAllocSize(read_mask_ty);
      }

      void layout(llvm::DataLayout const* lay) {
        m_layout = lay;
      }

      //
      // data members
      //

      Module_list modules;


    private:
      llvm::DataLayout const* m_layout = nullptr;
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

