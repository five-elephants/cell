#pragma once

#include "ir/codegen.h"
#include "ir/namespace.h"
#include "ir/builtins.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/PassManager.h>
#include <memory>
#include <unordered_map>

namespace sim {

  class Llvm_codegen : public ir::Codegen_base {
    public:
      Llvm_codegen();

      virtual std::shared_ptr<ir::Codeblock_if> make_codeblock(ir::Namespace const& ns);
      virtual void register_variable(std::shared_ptr<ir::Object> obj);
      virtual void emit();

      llvm::Value* make_constant(std::shared_ptr<ir::Type> type, int const& value) const {
        if(type == ir::Builtins::types["int"]) 
          return llvm::ConstantInt::get(m_context, llvm::APInt(64, value, true));

        return nullptr;
      }

      llvm::Type* get_type(std::shared_ptr<ir::Type> type) const; 
      llvm::Value* get_global(std::shared_ptr<ir::Object> object) const;
      void optimize(llvm::Function* func);

      std::shared_ptr<llvm::Module> module() { return m_module; }

    private:
      //typedef std::unordered_map< std::shared_ptr<ir::Object>,
              //std::shared_ptr<llvm::GlobalVariable> > Variable_map;
      typedef std::unordered_map< std::shared_ptr<ir::Object>,
              llvm::GlobalVariable* > Variable_map;

      llvm::LLVMContext& m_context;
      llvm::IRBuilder<> m_builder;
      std::shared_ptr<llvm::Module> m_module;
      llvm::FunctionPassManager m_fpm;
      Variable_map m_globals;

  };

}
