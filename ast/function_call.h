#pragma once

#include "ast/tree_base.h"
#include <vector>

namespace ast {

	class Function_call : public Tree_base {
		public:
			Function_call(std::vector<Node_if*> const& qualified_name);

			virtual void visit();

			std::vector<Node_if*>& name();
			std::vector<Node_if*> const& name() const;
			void expressions(std::vector<Node_if*>& nodes);
			std::vector<Node_if*>& expressions();
			std::vector<Node_if*> const& expressions() const { return m_expressions; }

		private:
			std::vector<Node_if*> m_name;
			std::vector<Node_if*> m_expressions;
	};

}

/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
