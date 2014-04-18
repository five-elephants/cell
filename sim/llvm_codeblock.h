#pragma once

#include "ir/codeblock.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace sim {

  class Codegen_visitor;

  class Llvm_codeblock : public ir::Codeblock_base {
    friend class Codegen_visitor;

    public:
      Llvm_codeblock(llvm::LLVMContext& context,
          llvm::IRBuilder<>& builder,
          std::shared_ptr<llvm::Module> module);

      virtual void scan_ast(ir::Namespace& enclosing_ns,
          ast::Node_if const& tree);

      virtual void append_predefined_objects(std::map<ir::Label, std::shared_ptr<ir::Object>> objects);

    private:
      llvm::LLVMContext& m_context;
      llvm::IRBuilder<>& m_builder;
      std::shared_ptr<llvm::Module> m_module;
      std::map<ir::Label, std::shared_ptr<ir::Object>> m_predefined_objects;
  };

}
