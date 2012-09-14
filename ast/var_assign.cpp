#include "ast/var_assign.h"

#include "gen/generator_if.h"

namespace ast {

	Var_assign::Var_assign(Node_if& identifier,
			Node_if& type,
			Node_if& expression)
		:	
			Node_base(),
			m_identifier(identifier),
			m_type(type),
			m_expression(expression) {
	}

	void
	Var_assign::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);
		m_identifier.set_generator(g);
		m_expression.set_generator(g);
		m_type.set_generator(g);
	}


	void
	Var_assign::visit() {
		get_generator().var_assign(*this);
	}

}

