#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <map>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "sim/llvm_namespace.h"
#include "ir/time.h"
#include "sim/memory.h"


namespace sim {

  struct Runset {

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

    struct Module {
      std::shared_ptr<Llvm_module> mod;
      Llvm_impl::Module::Frame this_in;
      Llvm_impl::Module::Frame this_out;
      Llvm_impl::Module::Read_mask read_mask;
      std::size_t read_mask_sz;
      llvm::StructLayout const* layout = nullptr;
      Process_list processes;
      Time_process_map periodicals;
      std::vector<Process_set> sensitivity;
      Process_set run_list;
      Process_schedule schedule;
    };

    typedef std::vector<Module> Module_list;


    //
    // member functions
    //

    static Module create_module(Memory& mem,
        llvm::ExecutionEngine* exe,
        std::shared_ptr<Llvm_module> mod);


    //
    // data members
    //

    Module_list modules;

  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

