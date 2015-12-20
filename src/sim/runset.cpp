#include "sim/runset.h"

#include <iostream>

namespace sim {

  void
  Runset::add_module(llvm::ExecutionEngine* exe,
      std::shared_ptr<Llvm_module> mod) {
    Module rv;
    rv.mod = mod;
    rv.this_in = allocate_module_frame(mod);
    std::fill(rv.this_in->begin(), rv.this_in->end(), 0);
    rv.this_out = allocate_module_frame(mod);
    std::fill(rv.this_out->begin(), rv.this_out->end(), 0);
    rv.this_prev = allocate_module_frame(mod);
    std::fill(rv.this_prev->begin(), rv.this_prev->end(), 0);
    rv.read_mask = allocate_read_mask(mod);
    rv.sensitivity.resize(mod->impl.mod_type->getNumElements());
    rv.layout = m_layout->getStructLayout(mod->impl.mod_type);

    for(auto proc : mod->processes) {
      Process p;
      p.function = proc->function->impl.code;
      p.exe_ptr = exe->getPointerToFunction(p.function);

      rv.processes.push_back(p);
      rv.run_list.insert(p);
    }

    for(auto proc : mod->periodicals) {
      Process p;
      p.function = proc->function->impl.code;
      p.exe_ptr = exe->getPointerToFunction(p.function);
      p.sensitive = false;

      rv.run_list.insert(p);

      rv.periodicals.insert(std::make_pair(proc->period, p));
      rv.schedule.insert(std::make_pair(proc->period, std::make_tuple(proc->period, p)));
    }

    for(auto proc : mod->onces) {
      Process p;
      p.function = proc->function->impl.code;
      p.exe_ptr = exe->getPointerToFunction(p.function);
      p.sensitive = false;

      rv.schedule.insert(std::make_pair(proc->time,
            std::make_tuple(ir::Time(0, ir::Time::ns), p)));
    }

    for(auto proc : mod->recurrents) {
      Process p;
      p.function = proc->function->impl.code;
      p.exe_ptr = exe->getPointerToFunction(p.function);
      p.sensitive = false;

      rv.recurrent_schedule.insert(
          std::make_pair(ir::Time(0, ir::Time::ns), p)
        );
    }

    modules.push_back(std::move(rv));

    // add submodules
    for(auto inst : mod->instantiations) {
      add_module(exe, inst.second->module);
    }
  }


  void
  Runset::setup_hierarchy() {
    for(auto& m : modules) {
      for(auto i : m.mod->instantiations) {
        ir::Label inst_name;
        std::shared_ptr<Llvm_instantiation> inst;

        std::tie(inst_name, inst) = i;

        auto obj = m.mod->objects.at(inst_name);
        auto index = obj->impl.struct_index;
        auto ofs = m.layout->getElementOffset(index);

        auto it = std::find_if(std::begin(modules),
            std::end(modules),
            [&inst](Module x) -> bool { return x.mod == inst->module; }
          );
        if( it == std::end(modules) )
          throw std::runtime_error("instantiated module not found in runset");

        uint64_t subptr_in = reinterpret_cast<uint64_t>(it->this_in->data());
        uint64_t subptr_out = reinterpret_cast<uint64_t>(it->this_out->data());
        std::copy_n((char*)&subptr_in,
            sizeof(subptr_in),
            m.this_in->data() + ofs);
        std::copy_n((char*)&subptr_out,
            sizeof(subptr_out),
            m.this_out->data() + ofs);
        std::copy_n((char*)&subptr_in,
            sizeof(subptr_in),
            m.this_prev->data() + ofs);
      }
    }
  }


  void
  Runset::call_init(llvm::ExecutionEngine* exe) {
    for(auto& m : modules) {
      auto mod = m.mod;

      // call __init__ if it exists
      auto init_f = mod->functions.find("__init__");
      if( init_f != mod->functions.end() ) {
        std::function<void(char*,char*,char*,char*)> init_func;
        void* ptr = exe->getPointerToFunction(init_f->second->impl.code);
        typedef void Func(char*,char*,char*,char*);
        init_func = reinterpret_cast<Func*>(ptr);
        init_func(m.this_out->data(),
            m.this_in->data(),
            m.this_prev->data(),
            m.read_mask->data());
        std::copy(m.this_out->begin(), m.this_out->end(), m.this_in->begin());
        std::copy(m.this_out->begin(), m.this_out->end(), m.this_prev->begin());
      }


      //{
        //std::cout << "memory contents after init:\n"
          //<< "this_in: " << std::hex;
        //std::cout << reinterpret_cast<int64_t>(m.this_in->data()) << ": ";
        //for(auto const& c : *(m.this_in))
          //std::cout << +c << ' ';
        //std::cout << "\nthis_out: " << std::hex;
        //std::cout << reinterpret_cast<int64_t>(m.this_out->data()) << ": ";
        //for(auto const& c : *(m.this_out))
          //std::cout << +c << ' ';
        //std::cout << std::endl;
      //}
    }
  }


  Runset::Module_frame
  Runset::allocate_module_frame(std::shared_ptr<Llvm_module> mod) {
    //auto mod_sz = m_layout->getTypeAllocSize(mod->impl.mod_type);
    auto mod_sz = module_frame_size(mod);
    return std::make_shared<std::vector<char>>(mod_sz);
  }


  Runset::Read_mask
  Runset::allocate_read_mask(std::shared_ptr<Llvm_module> mod) {
    //auto mod_type = mod->impl.mod_type;
    //auto read_mask_ty = llvm::ArrayType::get(
        //llvm::IntegerType::get(llvm::getGlobalContext(), 1),
        //mod_type->getNumElements()
      //);
    //auto sz = m_layout->getTypeAllocSize(read_mask_ty);
    auto sz = read_mask_size(mod);
    return std::make_shared<std::vector<char>>(sz);
  }


}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */

