#include "ast/for_expression.h"


namespace ast {

  For_expression::For_expression(Node_if& loop_var_id,
      Node_if& iterand,
      Node_if& body)
    : Tree_base(),
      m_loop_var_id(loop_var_id),
      m_iterand(iterand),
      m_body(body) {
  }

}


/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
