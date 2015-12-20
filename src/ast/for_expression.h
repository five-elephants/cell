#pragma once

#include "ast/tree_base.h"
#include "ast/identifier.h"

#include <string>


namespace ast {

  class For_expression : public Tree_base {
    public:
      For_expression(Node_if& loop_var_id, Node_if& iterand, Node_if& body);

      std::string loop_var_id() const {
        return dynamic_cast<Identifier const&>(m_loop_var_id).identifier();
      }
      Node_if const& iterand() const { return m_iterand; }
      Node_if const& body() const { return m_body; }

    private:
      Node_if& m_loop_var_id;
      Node_if& m_iterand;
      Node_if& m_body;
  };

}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
