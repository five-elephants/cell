#include "ast/function_def.h"

#include "ast/identifier.h"
#include "gen/util.h"
#include <iterator>
#include <algorithm>

namespace ast {

	static Identifier default_return_type = Identifier("void");

	Function_def::Function_def(Node_if& identifier)
		:	Node_base(),
			m_identifier(identifier),
 			m_return_type(default_return_type) {
	}

	Function_def::Function_def(Node_if& identifier, Node_if& return_type)
		:	Node_base(),
			m_identifier(identifier),
 			m_return_type(return_type) {
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
    gen::join_nodes_comma(begin(m_parameters), end(m_parameters), get_generator());

		get_generator().function_body(*this);
    gen::join_nodes_line(begin(m_body), end(m_body), get_generator());
		//for(auto i=begin(m_body); i!=end(m_body); ++i)
			//(*i)->visit();

		get_generator().function_end(*this);
	}


	void
	Function_def::append_parameter(Node_if& node) {
		m_parameters.push_back(&node);
	}

	void
	Function_def::append_parameter(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_parameters));
	}

	
	void
	Function_def::append_body(Node_if& node) {
		m_body.push_back(&node);
	}

	void
	Function_def::append_body(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_body));
	}

}
