#pragma once

#include "ir/codeblock.h"
#include "sim/llvm_codegen.h"
#include "ir/find.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace sim {

  class Codegen_visitor;
  class Module_codegen_visitor;

  class Llvm_codeblock : public ir::Codeblock_base {
    friend class Codegen_visitor;
    friend class Module_codegen_visitor;

    public:
      Llvm_codeblock(Llvm_codegen& parent,
          llvm::LLVMContext& context,
          llvm::IRBuilder<>& builder,
          std::shared_ptr<llvm::Module> module,
          ir::Namespace const& enclosing_ns);

      virtual void scan_ast(ast::Node_if const& tree);
      virtual void scan_ast_module(ast::Node_if const& tree);
      virtual void append_predefined_objects(std::map<ir::Label, std::shared_ptr<ir::Object>> objects);
      virtual void prototype(std::shared_ptr<ir::Function> func);
      virtual void process(std::shared_ptr<ir::Process> proc);
      virtual void enclosing_module(ir::Module* mod);

      llvm::Function* function() { return m_function; }

    private:
      Llvm_codegen& m_codegen;
      ir::Namespace const& m_enclosing_ns;
      ir::Module* m_enclosing_mod = nullptr;
      llvm::LLVMContext& m_context;
      llvm::IRBuilder<>& m_builder;
      std::shared_ptr<llvm::Module> m_module;
      std::map<ir::Label, std::shared_ptr<ir::Object>> m_predefined_objects;

      ir::Label m_function_name;
      std::shared_ptr<ir::Function> m_prototype;
      std::shared_ptr<ir::Process> m_process;
      llvm::FunctionType* m_function_type;
      llvm::Function* m_function;
      llvm::BasicBlock* m_bb;
      llvm::Value* m_read_mask = nullptr;

      template<typename T>
      llvm::Value* make_constant(ir::Label const& type_name, T const& value) const {
        auto the_type = ir::find_type(m_enclosing_ns, type_name);
        if( !the_type ) {
          return nullptr;
        }

        return m_codegen.make_constant(the_type, value);
      }

      llvm::AllocaInst* allocate_variable(ir::Label const& name, ir::Label const& type_name) const; 

      llvm::Type* get_type(ir::Label const& type_name) const {
        auto the_type = ir::find_type(m_enclosing_ns, type_name);
        if( !the_type )
          return nullptr;

        return m_codegen.get_type(the_type);
      }

      llvm::Value* get_global(ir::Label const& name) const {
        if( m_enclosing_mod ) {
          auto the_object = ir::find_object(*m_enclosing_mod, name);
          if( !the_object )
            return nullptr;

          return m_codegen.get_global(the_object);
        }

        return nullptr;
      }

      llvm::Function* get_function(ir::Label const& name) const {
        auto the_func = ir::find_function(m_enclosing_ns, name);
        if( !the_func )
          return nullptr;

        return m_codegen.get_function(the_func);
      }

      llvm::Value* get_module_object_out(ir::Label const& name);
      llvm::Value* get_module_object_in(ir::Label const& name);
      llvm::Value* create_function_call(ir::Label const& callee, std::vector<llvm::Value*> const& args);
  };

}
