#include "ast/compound.h"
#include "gen/generator_if.h"
#include <iterator>
#include <algorithm>

namespace ast {

	Compound::Compound() 
		:	Node_base() {
	}

	Compound::~Compound() {
	}

	void
	Compound::visit() {
		get_generator().compound(*this);
	}

	void
	Compound::set_generator(gen::Generator_if& g) {
		for(auto i : m_statements)
			i->set_generator(g);
	}

	void
	Compound::statements(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_statements));
	}

	std::vector<Node_if*>&
	Compound::statements() {
		return m_statements;
	}

}
