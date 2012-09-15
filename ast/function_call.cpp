#include "ast/function_call.h"

#include "gen/generator_if.h"
#include "ast/identifier.h"
#include <iterator>
#include <algorithm>

namespace ast {

	Function_call::Function_call(Node_if& identifier)
		:	Node_base(),
			m_identifier(identifier) {
	}

	Function_call::~Function_call() {
	}

	void
	Function_call::visit() {
		m_identifier.visit();
		for(auto i : m_expressions)
			i->visit();
	}

	void
	Function_call::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);

		for(auto i : m_expressions)
			i->set_generator(g);
	}

	void
	Function_call::expressions(std::vector<Node_if*>& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_expressions));
	}

}
