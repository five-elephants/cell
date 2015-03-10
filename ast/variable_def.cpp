#include "ast/variable_def.h"

#include "gen/generator_if.h"
#include "ast/identifier.h"
#include "ast/empty_expression.h"

namespace ast {

	Variable_def::Variable_def(Node_if& identifier)
		:	Tree_base(),
			m_identifier(identifier),
			m_type(new Identifier("void")),
			m_expression(new Empty_expression()),
			m_without_type(true),
			m_without_expression(true) {
		register_branches({&m_identifier, m_type.get(), m_expression.get()});
	}

	Variable_def::Variable_def(Node_if& identifier,
			Node_if& type,
			Node_if& expression)
		:	
			Tree_base(),
			m_identifier(identifier),
			m_type(&type),
			m_expression(&expression),
			m_without_type(false),
   			m_without_expression(false)	{
		register_branches({&m_identifier, m_type.get(), m_expression.get()});
	}

	Variable_def::~Variable_def() {
	}

	void
	Variable_def::visit() {
		get_generator().variable_def(*this);
	}

}


/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
