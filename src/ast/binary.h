#pragma once

#include "ast/tree_base.h"

namespace ast {

	class Binary : public Tree_base {
		public:
			Binary(Node_if& left, Node_if& right)
		 		:	Tree_base(),
					m_left(left),
					m_right(right) {
				register_branches({&m_left, &m_right});
			}

			Node_if& left() { return m_left; }
			Node_if const& left() const { return m_left; }
			Node_if& right() { return m_right; }
			Node_if const& right() const { return m_right; }

		private:
			Node_if& m_left;
			Node_if& m_right;
	};


}
