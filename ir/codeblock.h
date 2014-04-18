#pragma once

#include "namespace.h"
#include "ast/node_if.h"

namespace ir {

  class Codeblock_if {
    public:
      virtual void scan_ast(Namespace& enclosing_ns, 
          ast::Node_if const& tree) = 0;
  };


  class Codeblock_base : public Codeblock_if {
    public:
      virtual ~Codeblock_base();
  };


  class Null_codeblock : public Codeblock_base {
    public:
      virtual void scan_ast(Namespace& enclosing_ns,
          ast::Node_if const& tree);

  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
