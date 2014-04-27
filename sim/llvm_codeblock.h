#pragma once

#include "ir/codeblock.h"
#include "sim/llvm_codegen.h"
#include "ir/find.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace sim {

  class Codegen_visitor;

  class Llvm_codeblock : public ir::Codeblock_base {
    friend class Codegen_visitor;

    public:
      Llvm_codeblock(Llvm_codegen const& parent,
          llvm::LLVMContext& context,
          llvm::IRBuilder<>& builder,
          std::shared_ptr<llvm::Module> module,
          ir::Namespace const& enclosing_ns);

      virtual void scan_ast(ast::Node_if const& tree);

      virtual void append_predefined_objects(std::map<ir::Label, std::shared_ptr<ir::Object>> objects);


    private:
      Llvm_codegen const& m_codegen;
      ir::Namespace const& m_enclosing_ns;
      llvm::LLVMContext& m_context;
      llvm::IRBuilder<>& m_builder;
      std::shared_ptr<llvm::Module> m_module;
      std::map<ir::Label, std::shared_ptr<ir::Object>> m_predefined_objects;

      llvm::FunctionType* m_function_type;
      llvm::Function* m_function;
      llvm::BasicBlock* m_bb;

      template<typename T>
      llvm::Value* make_constant(ir::Label const& type_name, T const& value) const {
        auto the_type = ir::find_type(m_enclosing_ns, type_name);
        if( !the_type ) {
          return nullptr;
        }

        return m_codegen.make_constant(the_type, value);
      }
  };

}
