#include "ast/while_expression.h"

namespace ast {

  While_expression::While_expression(Node_if& expression, Node_if& body)
    : Tree_base(),
      m_expression(expression),
      m_body(body) {
    register_branches({&m_expression, &m_body});
  }

}
