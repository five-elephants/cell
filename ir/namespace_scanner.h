#pragma once

#include "ast/ast.h"
#include "ast/visitor.h"
#include "ir/namespace.h"
#include "ir/codegen.h"

//#include <iostream>

namespace ir {

  class Namespace_scanner : public ast::Visitor_base {
    public:
      Namespace_scanner(Namespace& ns, Codegen_if& codegen) 
        : m_ns(ns),
          m_codegen(codegen),
          m_root(true) {
      }

      virtual bool enter(ast::Node_if const& node); 
      virtual bool visit(ast::Node_if const& node);

    protected:
      bool m_root;
      Namespace& m_ns;
      Codegen_if& m_codegen;

      std::shared_ptr<Module> insert_module(ast::Module_def const& mod);
      std::shared_ptr<Namespace> insert_namespace(ast::Namespace_def const& ns);
      std::shared_ptr<Socket> insert_socket(ast::Socket_def const& sock);
      std::shared_ptr<Function> insert_function(ast::Function_def const& func);

      virtual std::shared_ptr<Codeblock_if> make_codeblock();
  };

}
