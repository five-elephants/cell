#include "ast/if_statement.h"


namespace ast {

	If_statement::If_statement(Node_if& condition, Node_if& body)
		:	Tree_base(),
			m_condition(condition),
			m_body(body) {
    register_branches({&m_condition, &m_body});
	}

}
