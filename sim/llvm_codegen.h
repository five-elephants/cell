#pragma once

#include "ir/codegen.h"
#include "ir/namespace.h"
#include "ir/builtins.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>

namespace sim {

  class Llvm_codegen : public ir::Codegen_base {
    public:
      Llvm_codegen();

      virtual std::shared_ptr<ir::Codeblock_if> make_codeblock(ir::Namespace const& ns);
      virtual void emit();

      //template<typename T>
      //llvm::Value* make_constant(std::shared_ptr<ir::Type> type, T const& value) {
        //return nullptr;
      //}

      llvm::Value* make_constant(std::shared_ptr<ir::Type> type, int const& value) const {
        if(type == ir::Builtins::types["int"]) 
          return llvm::ConstantInt::get(m_context, llvm::APInt(64, value, true));

        return nullptr;
      }

    private:
      llvm::LLVMContext& m_context;
      llvm::IRBuilder<> m_builder;
      std::shared_ptr<llvm::Module> m_module;
  };

}
