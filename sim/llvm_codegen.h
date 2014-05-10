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
      virtual void register_global(std::shared_ptr<ir::Object> obj);
      virtual void register_function(std::shared_ptr<ir::Function> func, llvm::Function* proto);
      virtual void register_module_init(ir::Module* mod, llvm::Function* func);
      virtual void emit();

      llvm::Value* make_constant(std::shared_ptr<ir::Type> type, int const& value) const {
        if(type == ir::Builtins::types["int"]) 
          return llvm::ConstantInt::get(m_context, llvm::APInt(64, value, true));

        return nullptr;
      }

      llvm::Type* get_type(std::shared_ptr<ir::Type> type) const; 
      llvm::Value* get_global(std::shared_ptr<ir::Object> object) const;
      llvm::Function* get_function(std::shared_ptr<ir::Function> func) const;
      void optimize(llvm::Function* func);

      std::shared_ptr<llvm::Module> module() { return m_module; }

      llvm::Type* get_module_type(ir::Module* mod) const {
        return m_module_types.at(mod);
      }


      llvm::Function* get_module_init(ir::Module* mod) const {
        return m_module_inits.at(mod);
      }

      void add_module_type(ir::Module* mod, llvm::Type* type) {
        m_module_types[mod] = type;
      }

    private:
      //typedef std::unordered_map< std::shared_ptr<ir::Object>,
              //std::shared_ptr<llvm::GlobalVariable> > Variable_map;
      typedef std::unordered_map< std::shared_ptr<ir::Object>,
              llvm::GlobalVariable* > Variable_map;

      typedef std::unordered_map< ir::Module*, llvm::Type* > Module_type_map;
      typedef std::unordered_map< ir::Module*, llvm::Function* > Module_init_map;
      typedef std::unordered_map< ir::Function*, llvm::Function* > Function_map;

      llvm::LLVMContext& m_context;
      llvm::IRBuilder<> m_builder;
      std::shared_ptr<llvm::Module> m_module;
      llvm::FunctionPassManager m_fpm;
      Variable_map m_globals;
      Module_type_map m_module_types;
      Module_init_map m_module_inits;
      Function_map m_functions;

  };

}

/* vim: set et fenc=utf-8 ff=unix sts=0 sw=2 ts=2 : */
