#include "ast/while_statement.h"
#include "gen/generator_if.h"

namespace ast {

  While_statement::While_statement(Node_if& expression, Node_if& body)
    : Tree_base(),
      m_expression(expression),
      m_body(body) {
    register_branches({&m_expression, &m_body});
  }

  void
  While_statement::visit() {
    get_generator().while_statement(*this);
  }


}
