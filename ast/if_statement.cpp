#include "ast/if_statement.h"
#include "gen/generator_if.h"


namespace ast {

	If_statement::If_statement(Node_if& condition, Node_if& body)
		:	Tree_base(),
			m_condition(condition),
			m_body(body) {
    register_branches({&m_condition, &m_body});
	}

	If_statement::~If_statement() {
	}


	void
	If_statement::visit() {
		get_generator().if_statement(*this);
	}

}
