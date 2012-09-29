#include "ast/while_statement.h"
#include "gen/generator_if.h"

namespace ast {

  While_statement::While_statement(Node_if& expression, Node_if& body)
    : Node_base(),
      m_expression(expression),
      m_body(body) {
  }

  While_statement::~While_statement() {
  }

  void
  While_statement::visit() {
    get_generator().while_statement(*this);
  }

  void
  While_statement::set_generator(gen::Generator_if& g) {
    Node_base::set_generator(g);
    m_expression.set_generator(g);
    m_body.set_generator(g);
  }


}
