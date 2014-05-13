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
      virtual void register_process(std::shared_ptr<ir::Process> proc, llvm::Function* func);
      virtual void register_module_ctor(ir::Module* mod, llvm::Function* func);
      virtual void register_module_init(ir::Module* mod, llvm::Function* func);
      virtual void emit();
      virtual void create_setup(std::shared_ptr<ir::Module> toplevel);

      llvm::Value* make_constant(std::shared_ptr<ir::Type> type, int const& value) const {
        if(type == ir::Builtins::types["int"]) 
          return llvm::ConstantInt::get(m_context, llvm::APInt(64, value, true));

        return nullptr;
      }

      llvm::Value* make_constant(std::shared_ptr<ir::Type> type,
          std::string const& value) {
        if( type == ir::Builtins::types["string"] )
          //return llvm::ConstantDataArray::getString(m_context, value);
          return m_builder.CreateGlobalStringPtr(value, "string");

        return nullptr;
      }

      llvm::Type* get_type(std::shared_ptr<ir::Type> type) const; 
      llvm::Value* get_global(std::shared_ptr<ir::Object> object) const;
      llvm::Function* get_function(std::shared_ptr<ir::Function> func) const;
      llvm::Function* get_process(std::shared_ptr<ir::Process> process) const;
      void optimize(llvm::Function* func);

      std::shared_ptr<llvm::Module> module() { return m_module; }

      llvm::Type* get_module_type(ir::Module* mod) const {
        return m_module_types.at(mod);
      }


      llvm::Function* get_module_ctor(ir::Module* mod) const {
        return m_module_ctors.at(mod);
      }


      llvm::Function* get_module_init(ir::Module* mod) const {
        return m_module_inits.at(mod);
      }


      void add_module_type(ir::Module* mod, llvm::Type* type) {
        m_module_types[mod] = type;
      }




      llvm::GlobalVariable* root() { return m_root; }

    private:
      //typedef std::unordered_map< std::shared_ptr<ir::Object>,
              //std::shared_ptr<llvm::GlobalVariable> > Variable_map;
      typedef std::unordered_map< std::shared_ptr<ir::Object>,
              llvm::GlobalVariable* > Variable_map;

      typedef std::unordered_map< ir::Type*, llvm::Type* > Type_map;
      typedef std::unordered_map< ir::Module*, llvm::Type* > Module_type_map;
      typedef std::unordered_map< ir::Module*, llvm::Function* > Module_init_map;
      typedef std::unordered_map< ir::Function*, llvm::Function* > Function_map;
      typedef std::unordered_map< ir::Process*, llvm::Function* > Process_map;

      llvm::LLVMContext& m_context;
      llvm::IRBuilder<> m_builder;
      std::shared_ptr<llvm::Module> m_module;
      llvm::FunctionPassManager m_fpm;
      Variable_map m_globals;
      Type_map m_type_map;
      Module_type_map m_module_types;
      Module_init_map m_module_ctors;
      Module_init_map m_module_inits;
      Function_map m_functions;
      Process_map m_processes;
      llvm::GlobalVariable* m_root = nullptr;


      void define_builtin_types();
      void declare_runtime_functions();
  };

}

/* vim: set et fenc=utf-8 ff=unix sts=0 sw=2 ts=2 : */
