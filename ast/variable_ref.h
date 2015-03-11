#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Variable_ref : public Tree_base {
    public:
      Variable_ref(Node_if& expression);

      virtual void visit();

      Node_if const& expression() const { return m_expression; }

    private:
      Node_if& m_expression;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
