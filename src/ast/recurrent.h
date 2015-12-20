#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Recurrent : public Tree_base {
    public:
      Recurrent(Node_if& time_id, Node_if& expression)
        : Tree_base(),
          m_time_id(time_id),
          m_expression(expression) {
        register_branches({&m_time_id, &m_expression});
      }

      Node_if const& time_id() const { return m_time_id; }
      Node_if const& expression() const { return m_expression; }

    private:
      Node_if& m_time_id;
      Node_if& m_expression;
  };

}

/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
