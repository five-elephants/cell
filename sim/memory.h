#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "sim/llvm_namespace.h"


namespace sim {

  class Memory {
    public:
      Memory();

      Llvm_impl::Module::Frame allocate_module_frame(std::shared_ptr<Llvm_module> mod);
      Llvm_impl::Module::Read_mask allocate_read_mask(std::shared_ptr<Llvm_module> mod);

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

    private:
      llvm::DataLayout const* m_layout = nullptr;
  };

}



/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
