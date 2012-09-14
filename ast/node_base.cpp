#include "ast/node_base.h"

#include "gen/generator_if.h"

namespace ast {

	Node_base::Node_base(gen::Generator_if& generator)
		:	m_generator(generator) {
	}

	void
	Node_base::set_generator(gen::Generator_if& generator) {
		m_generator = generator;
	}

	gen::Generator_if&
	Node_base::get_generator() {
		return m_generator;
	}

}
