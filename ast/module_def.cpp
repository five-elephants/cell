#include "ast/module_def.h"

#include "gen/generator_if.h"
#include "gen/util.h"
#include <algorithm>
#include <iterator>


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
		gen::join_nodes_line(begin(m_elements), end(m_elements), get_generator());
		//for(auto i=begin(m_elements); i!=end(m_elements); ++i) {
			//(*i)->visit();
		//}
		get_generator().module_end(*this);
	}

  void
  Module_def::visit(std::function<void(Node_if const&)> cb) const {
    Node_base::visit(cb);
    m_identifier.visit(cb);
    for(auto i : m_elements)
      i->visit(cb);
  }

	void
	Module_def::append(Node_if& node) {
		m_elements.push_back(&node);
	}

	void
	Module_def::append(std::vector<Node_if*> const& nodes) {
		std::copy(begin(nodes), end(nodes), std::back_inserter(m_elements));
	}

}
