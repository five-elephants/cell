#pragma once

#include "ast/node_base.h"

namespace ast {

  class While_statement : public Node_base {
    public:
      While_statement(Node_if& expression, Node_if& body);
      virtual ~While_statement();

      virtual void visit();
      virtual void set_generator(gen::Generator_if& g);

      Node_if& expression() { return m_expression; }
      Node_if& body() { return m_body; }

    private:
      Node_if& m_expression;
      Node_if& m_body;
  };

}
