#include "ast/unit.h"

#include "gen/generator_if.h"
#include "gen/util.h"
#include <algorithm>
#include <iterator>


namespace ast {

	void
	Unit::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);

		for(auto i=begin(m_elements); i!=end(m_elements); ++i) {
			(*i)->set_generator(g);
		}
	}

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

