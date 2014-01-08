#pragma once

#include "ast/tree_base.h"

#include <string>
#include <vector>
#include <initializer_list>
#include <algorithm>

namespace ast {

	class Statement : public Tree_base {
		public:
			Statement(std::string const& macro_name, std::initializer_list<Node_if*> objs)
				:	Tree_base(),
					m_macro_name(macro_name) {
				m_objs.resize(objs.size());
				std::copy(begin(objs), end(objs), begin(m_objs));
        register_branch_lists({&m_objs});
			}

			virtual void visit() {
				get_generator().generic_statement(m_macro_name, m_objs);
			}

		protected:
			std::string m_macro_name;
			std::vector<Node_if*> m_objs;
	};

}

