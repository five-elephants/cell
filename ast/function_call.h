#pragma once

#include "ast/tree_base.h"
#include <vector>

namespace ast {

	class Function_call : public Tree_base {
		public:
			Function_call(Node_if& identifier);

			virtual void visit();

			Node_if& identifier();
			void expressions(std::vector<Node_if*>& nodes);
			std::vector<Node_if*>& expressions();

		private:
			Node_if& m_identifier;
			std::vector<Node_if*> m_expressions;
	};

}

/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
