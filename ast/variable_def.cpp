#include "ast/variable_def.h"

#include "gen/generator_if.h"
#include "ast/identifier.h"
#include "ast/empty_expression.h"

namespace ast {

	Variable_def::Variable_def(Node_if& identifier)
		:	Node_base(),
			m_identifier(identifier),
			m_type(new Identifier("void")),
			m_expression(new Empty_expression()),
 			m_is_type_owner(true),
			m_is_expression_owner(true)	{
	}

	Variable_def::Variable_def(Node_if& identifier,
			Node_if& type,
			Node_if& expression)
		:	
			Node_base(),
			m_identifier(identifier),
			m_type(&type),
			m_expression(&expression) {
	}

	Variable_def::~Variable_def() {
		if( m_is_expression_owner )
			delete m_expression;

		if( m_is_type_owner )
			delete m_type;
	}

	void
	Variable_def::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);
		m_identifier.set_generator(g);
		m_expression->set_generator(g);
		m_type->set_generator(g);
	}


	void
	Variable_def::visit() {
		get_generator().variable_def(*this);
	}

}

