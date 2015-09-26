#pragma once

#include "ast/tree_base.h"

namespace ast {

  class While_expression: public Tree_base {
    public:
      While_expression(Node_if& expression, Node_if& body);

      Node_if const& expression() const { return m_expression; }
      Node_if const& body() const { return m_body; }

    private:
      Node_if& m_expression;
      Node_if& m_body;
  };

}
