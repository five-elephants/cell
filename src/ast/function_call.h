#pragma once

#include "ast/node_if.h"
#include "ast/qualified_name.h"
#include "ast/tree_base.h"
#include <vector>

namespace ast {

	class Function_call : public Tree_base {
		public:
			Function_call(Node_if& qualified_name);

			std::vector<std::string> name() const {
				auto qn = dynamic_cast<Qualified_name const&>(m_name);
				return qn.name();
			}

			void expressions(std::vector<Node_if*>& nodes);
			std::vector<Node_if*>& expressions();
			std::vector<Node_if*> const& expressions() const { return m_expressions; }

		private:
			Node_if& m_name;
			std::vector<Node_if*> m_expressions;
	};

}

/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
