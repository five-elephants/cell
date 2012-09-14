#include "ast/variable_def.h"

#include "gen/generator_if.h"

namespace ast {

	Variable_def::Variable_def(Node_if& identifier,
			Node_if& type,
			Node_if& expression)
		:	
			Node_base(),
			m_identifier(identifier),
			m_type(type),
			m_expression(expression) {
	}

	void
	Variable_def::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);
		m_identifier.set_generator(g);
		m_expression.set_generator(g);
		m_type.set_generator(g);
	}


	void
	Variable_def::visit() {
		get_generator().variable_def(*this);
	}

}

