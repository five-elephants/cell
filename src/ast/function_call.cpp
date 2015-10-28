#include "ast/function_call.h"

#include "ast/identifier.h"
#include <iterator>
#include <algorithm>

namespace ast {

	Function_call::Function_call(Node_if& name)
		:	Tree_base(),
			m_name(name) {
		register_branches({&m_name});
		register_branch_lists({&m_expressions});
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
