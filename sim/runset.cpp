#include "sim/runset.h"


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

    modules.push_back(std::move(rv));
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

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

