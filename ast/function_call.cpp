#include "ast/function_call.h"

#include "gen/generator_if.h"
#include "ast/identifier.h"
#include <iterator>
#include <algorithm>

namespace ast {

	Function_call::Function_call(std::vector<Node_if*> const& name)
		:	Tree_base(),
			m_name(name) {
        register_branch_lists({&m_name, &m_expressions});
	}

	void
	Function_call::visit() {
		get_generator().function_call(*this);
		//m_identifier.visit();
		//for(auto i : m_expressions)
			//i->visit();
	}

    std::vector<Node_if*>&
	Function_call::name() {
		return m_name;
	}

    std::vector<Node_if*> const&
	Function_call::name() const {
		return m_name;
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
