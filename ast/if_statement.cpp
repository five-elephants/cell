#include "ast/if_statement.h"
#include "gen/generator_if.h"


namespace ast {

	If_statement::If_statement(Node_if& condition, Node_if& body)
		:	Node_base(),
			m_condition(condition),
			m_body(body) {
	}

	If_statement::~If_statement() {
	}


	void
	If_statement::visit() {
		get_generator().if_statement(*this);
	}

  void
  If_statement::visit(std::function<void(Node_if const&)> cb) const {
    Node_base::visit(cb);
    m_condition.visit(cb);
    m_body.visit(cb);
    if( m_else_body != nullptr )
      m_else_body->visit(cb);
  }

	void
	If_statement::set_generator(gen::Generator_if& g) {
    Node_base::set_generator(g);
		m_condition.set_generator(g);
		m_body.set_generator(g);
    if( has_else_body() )
      m_else_body->set_generator(g);
	}

}
