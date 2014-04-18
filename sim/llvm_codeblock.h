#pragma once

#include "ir/codeblock.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace sim {

  class Llvm_codeblock : public ir::Codeblock_base {
    public:
      Llvm_codeblock(llvm::LLVMContext& context,
          llvm::IRBuilder<>& builder,
          std::shared_ptr<llvm::Module> module);

      virtual void scan_ast(ir::Namespace& enclosing_ns,
          ast::Node_if const& tree);

    private:
      llvm::LLVMContext& m_context;
      llvm::IRBuilder<>& m_builder;
      std::shared_ptr<llvm::Module> m_module;
  };

}
