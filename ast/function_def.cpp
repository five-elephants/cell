#include "ast/function_def.h"

namespace ast {

	Function_def::Function_def(Node_if& identifier, gen::Generator_if& generator)
		:	Node_base(generator),
			m_identifier(identifier) {
	}


	void
	Function_def::set_generator(gen::Generator_if& g) {
		Node_base::set_generator(g);

		for(auto i=begin(m_parameters); i!=end(m_parameters); ++i)
			(*i)->set_generator(g);

		for(auto i=begin(m_body); i!=end(m_body); ++i)
			(*i)->set_generator(g);
	}


	void
	Function_def::visit() {
		get_generator().function_begin(*this);
		for(auto i=begin(m_parameters); i!=end(m_parameters); ++i)
			(*i)->visit();

		get_generator().function_body(*this);
		for(auto i=begin(m_body); i!=end(m_body); ++i)
			(*i)->visit();

		get_generator().function_end(*this);
	}


	void
	Function_def::append_parameter(Node_if& node) {
		m_parameters.push_back(&node);
	}

	
	void
	Function_def::append_body(Node_if& node) {
		m_body.push_back(&node);
	}

}
