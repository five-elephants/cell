#pragma once

#include "ast/tree_base.h"
#include <vector>

namespace ast {

	class Unit : public Tree_base {
		public:
			Unit()
				:	Tree_base() {
				register_branch_lists({&m_elements});
			}

			virtual ~Unit() {};

			virtual void visit();

			void append(Node_if& node);
			void append(std::vector<Node_if*> const& nodes);

		private:
			std::vector<Node_if*> m_elements;
	};

}


/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
