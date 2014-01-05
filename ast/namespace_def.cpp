#include "ast/namespace_def.h"

#include <algorithm>

namespace ast {

  Namespace_def::Namespace_def(Node_if& identifier)
    : Tree_base(),
      m_identifier(identifier) {
    register_branches({&m_identifier});
    register_branch_lists({&m_elements});
  }


  
	void
	Namespace_def::visit() {
	}


	void
	Namespace_def::append(Node_if& node) {
		m_elements.push_back(&node);
	}


	void
	Namespace_def::append(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_elements));
	}

}
