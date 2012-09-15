#include "ast/variable_def.h"

#include "gen/generator_if.h"
#include "ast/identifier.h"
#include "ast/empty_expression.h"

namespace ast {
	static Identifier default_type = Identifier("void");
	static Empty_expression default_expression = Empty_expression();


	Variable_def::Variable_def(Node_if& identifier)
		:	Node_base(),
			m_identifier(identifier),
			m_type(default_type),
			m_expression(default_expression) {
	}

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

