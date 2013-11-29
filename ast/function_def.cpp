#include "ast/function_def.h"

#include "ast/identifier.h"
#include "gen/util.h"
#include <iterator>
#include <algorithm>

namespace ast {

	static Identifier default_return_type = Identifier("void");

	Function_def::Function_def(Node_if& identifier)
		:	Tree_base(),
			m_identifier(identifier),
 			m_return_type(default_return_type) {
        register_branches({&m_identifier, &default_return_type});
        register_branch_lists({&m_parameters, &m_body});
	}

	Function_def::Function_def(Node_if& identifier, Node_if& return_type)
		:	Tree_base(),
			m_identifier(identifier),
 			m_return_type(return_type) {
        register_branches({&identifier, &return_type});
        register_branch_lists({&m_parameters, &m_body});
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


/* vim: set et fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
