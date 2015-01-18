#pragma once 

#include "ast/scanner_base.h"

#include "llvm_namespace.h"
#include "ast/ast.h"


namespace sim {

  class Llvm_function_scanner : public ast::Scanner_base<Llvm_function_scanner> {
    public:
      Llvm_function_scanner(Llvm_namespace& ns, Llvm_function& function);
      Llvm_function_scanner(Llvm_module& ns, Llvm_function& function);


    private:
      typedef std::unordered_map<ast::Node_if const*, llvm::Value*> Node_value_map;
      typedef std::unordered_map<ir::Label, llvm::AllocaInst*> Name_value_map;

      Llvm_namespace& m_ns;
      Llvm_module* m_mod = nullptr; 
      Llvm_function& m_function;
      llvm::IRBuilder<> m_builder;
      Node_value_map m_values;
      Name_value_map m_named_values;


      void init_function();
      void init_scanner();
      llvm::FunctionType* get_function_type(Llvm_function const& function) const;
      llvm::ArrayType* read_mask_type() const;


      // scanner callbacks
      virtual bool insert_return(ast::Return_statement const& node);
      virtual bool insert_variable_ref(ast::Variable_ref const& node);
      virtual bool insert_literal_int(ast::Literal<int> const& node);
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
