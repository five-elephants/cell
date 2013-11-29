#include "ast/function_call.h"

#include "gen/generator_if.h"
#include "ast/identifier.h"
#include <iterator>
#include <algorithm>

namespace ast {

	Function_call::Function_call(Node_if& identifier)
		:	Tree_base(),
			m_identifier(identifier) {
    register_branches({&m_identifier});
    register_branch_lists({&m_expressions});
	}

	Function_call::~Function_call() {
	}

	void
	Function_call::visit() {
		get_generator().function_call(*this);
		//m_identifier.visit();
		//for(auto i : m_expressions)
			//i->visit();
	}


	Node_if&
	Function_call::identifier() {
		return m_identifier;
	}

	void
	Function_call::expressions(std::vector<Node_if*>& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_expressions));
	}

	std::vector<Node_if*>&
	Function_call::expressions() {
		return m_expressions;
	}

}
