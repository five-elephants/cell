#include "ast/module_def.h"

#include <algorithm>
#include <iterator>


namespace ast {

  Module_def::Module_def(Node_if& identifier)
    :	Tree_base(),
      m_socket(nullptr),
      m_identifier(identifier) {
    register_branches({&m_identifier});
    register_branch_lists({&m_elements});
  }

	void
	Module_def::append(Node_if& node) {
		m_elements.push_back(&node);
	}

	void
	Module_def::append(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_elements));
	}

}
