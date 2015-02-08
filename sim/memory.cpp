#include "sim/memory.h"


namespace sim {

  Memory::Memory() {
  }


  Llvm_impl::Module::Frame
  Memory::allocate_module_frame(std::shared_ptr<Llvm_module> mod) {
    //auto mod_sz = m_layout->getTypeAllocSize(mod->impl.mod_type);
    auto mod_sz = module_frame_size(mod);
    return Llvm_impl::Module::Frame(new char [mod_sz]);
  }


  Llvm_impl::Module::Read_mask
  Memory::allocate_read_mask(std::shared_ptr<Llvm_module> mod) {
    //auto mod_type = mod->impl.mod_type;
    //auto read_mask_ty = llvm::ArrayType::get(
        //llvm::IntegerType::get(llvm::getGlobalContext(), 1),
        //mod_type->getNumElements()
      //);
    //auto sz = m_layout->getTypeAllocSize(read_mask_ty);
    auto sz = read_mask_size(mod);
    return Llvm_impl::Module::Read_mask(new char [sz]);
  }


}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
