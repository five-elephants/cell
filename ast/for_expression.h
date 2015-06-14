#pragma once

#include "ast/tree_base.h"


namespace ast {

  class For_expression : public Tree_base {
    public:
      For_expression(Node_if& loop_var_id, Node_if& iterand, Node_if& body);

    private:
      Node_if& m_loop_var_id;
      Node_if& m_iterand;
      Node_if& m_body;
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
