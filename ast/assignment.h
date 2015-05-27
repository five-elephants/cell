#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Assignment : public Tree_base {
    public:
      Assignment(Node_if& identifier, Node_if& expression);

      Node_if const& identifier() const { return m_identifier; }
      Node_if const& expression() const { return m_expression; }

    private:
      Node_if& m_identifier;
      Node_if& m_expression;
  };

}
