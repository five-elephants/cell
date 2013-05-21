#pragma once

#include "ast/node_base.h"

#include <string>
#include <vector>
#include <initializer_list>
#include <algorithm>

namespace ast {

	class Statement : public Node_base {
		public:
			Statement(std::string const& macro_name, std::initializer_list<Node_if*> objs)
				:	Node_base(),
					m_macro_name(macro_name) {
				m_objs.resize(objs.size());
				std::copy(begin(objs), end(objs), begin(m_objs));
			}
			virtual ~Statement() {};

			virtual void visit() {
				get_generator().generic_statement(m_macro_name, m_objs);
			}

			virtual void set_generator(gen::Generator_if& g) {
				Node_base::set_generator(g);

				for(auto i : m_objs)
					i->set_generator(g);
			}

		protected:
			std::string m_macro_name;
			std::vector<Node_if*> m_objs;
	};

}

