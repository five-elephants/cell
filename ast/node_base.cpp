#include "ast/node_base.h"

#include "gen/generator_if.h"
#include "gen/gen_text.h"
#include <iostream>

namespace ast {

	gen::Text_generator _default_generator(std::cout);
	gen::Generator_if& Node_base::default_generator = _default_generator;


	Node_base::Node_base()
		:	m_generator(default_generator) {
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
