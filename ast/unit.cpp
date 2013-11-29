#include "ast/unit.h"

#include "gen/generator_if.h"
#include "gen/util.h"
#include <algorithm>
#include <iterator>


namespace ast {

	void
	Unit::visit() {
		gen::join_nodes_line(begin(m_elements), end(m_elements), get_generator());
	}

	void
	Unit::append(Node_if& node) {
		m_elements.push_back(&node);
	}

	void
	Unit::append(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_elements));
	}

}

