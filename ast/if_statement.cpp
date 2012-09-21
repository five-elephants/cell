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
	If_statement::set_generator(gen::Generator_if& g) {
		m_condition.set_generator(g);
		m_body.set_generator(g);
	}

}
