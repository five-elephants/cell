#include "ast/assignment.h"

namespace ast {
 
  Assignment::Assignment(Node_if& identifier, Node_if& expression)
    : Tree_base(),
      m_identifier(identifier),
      m_expression(expression) {
    register_branches({&m_identifier, &m_expression});
  }

}