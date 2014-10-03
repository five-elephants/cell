#include "ast/compound.h"
#include "gen/generator_if.h"
#include <iterator>
#include <algorithm>

namespace ast {

	Compound::Compound() 
		:	Tree_base() {
    register_branch_lists({&m_statements});
	}

	void
	Compound::visit() {
		get_generator().compound(*this);
	}

	void
	Compound::statements(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_statements));
	}

	std::vector<Node_if*>&
	Compound::statements() {
		return m_statements;
	}

	std::vector<Node_if*> const&
	Compound::statements() const {
		return m_statements;
	}
}
