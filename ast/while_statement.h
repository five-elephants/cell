#pragma once

#include "ast/tree_base.h"

namespace ast {

  class While_statement : public Tree_base {
    public:
      While_statement(Node_if& expression, Node_if& body);
      virtual ~While_statement();

      virtual void visit();

      Node_if& expression() { return m_expression; }
      Node_if& body() { return m_body; }

    private:
      Node_if& m_expression;
      Node_if& m_body;
  };

}
