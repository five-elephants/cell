#include "ast/module_def.h"

#include "gen/generator_if.h"

namespace ast {

	void
	Module_def::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);
		m_identifier.set_generator(g);

		for(auto i=begin(m_elements); i!=end(m_elements); ++i) {
			(*i)->set_generator(g);
		}
	}


	void
	Module_def::visit() {
		get_generator().module_begin(*this);
		for(auto i=begin(m_elements); i!=end(m_elements); ++i) {
			(*i)->visit();
		}
		get_generator().module_end(*this);
	}


	void
	Module_def::append(Node_if& node) {
		m_elements.push_back(&node);
	}

}
