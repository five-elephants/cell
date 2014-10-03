#pragma once

#include "ast/tree_base.h"
#include <vector>

namespace ast {

	class Compound : public Tree_base {
		public:
			Compound();

			virtual void visit();

			void statements(std::vector<Node_if*> const& nodes);
			std::vector<Node_if*>& statements();
			std::vector<Node_if*> const& statements() const;

		private:
			std::vector<Node_if*> m_statements;
	};

}
