#pragma once

#include "ir/namespace_scanner.h"

#include "sim/llvm_namespace.h"


namespace sim {

  class Llvm_namespace_scanner : public ir::Namespace_scanner<Llvm_impl> {
    public:
      Llvm_namespace_scanner(Llvm_namespace& ns);


    protected:
      virtual bool insert_namespace(ast::Namespace_def const& ns);
      virtual bool insert_module(ast::Module_def const& mod);
      virtual bool insert_function(ast::Function_def const& func);
      virtual bool insert_socket(ast::Socket_def const& node);
      virtual bool insert_constant(ast::Constant_def const& node);
      virtual bool insert_table(ast::Table_def const& node);

      virtual std::shared_ptr<Llvm_type> create_array_type(ast::Array_type const& node);
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
