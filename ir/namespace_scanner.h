#pragma once

#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "ir/namespace.h"
#include "ir/codegen.h"

//#include <iostream>

namespace ir {

  class Namespace_scanner : public ast::Scanner_base<Namespace_scanner> {
    public:
      Namespace_scanner(Namespace& ns, Codegen_if& codegen);

      virtual bool enter(ast::Node_if const& node); 
      virtual bool visit(ast::Node_if const& node);

    protected:
      bool m_root;
      Namespace& m_ns;
      Codegen_if& m_codegen;

      virtual bool insert_module(ast::Module_def const& mod);
      virtual bool insert_namespace(ast::Namespace_def const& ns);
      virtual bool insert_socket(ast::Socket_def const& sock);
      virtual bool insert_function(ast::Function_def const& func);

      virtual std::shared_ptr<Codeblock_if> make_codeblock();
  };

}
