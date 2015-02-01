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
			void return_last(bool v) { m_return_last = v; }
			bool return_last() const { return m_return_last; }


		private:
			std::vector<Node_if*> m_statements;
			bool m_return_last = false;
	};

}
