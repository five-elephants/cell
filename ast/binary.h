#pragma once

#include "ast/node_if.h"

namespace ast {

	template<typename Left, typename Right>
	class Binary : public Node_if {
		public:
			Binary(Left& left, Right& right)
		 		:	m_left(left),
					m_right(right) {
			}

			virtual ~Binary() {
			}

			Left const& left() const { return m_left; }
			Right const& right() const { return m_right; }

		private:
			Left& m_left;
			Right& m_right;
	};


}
