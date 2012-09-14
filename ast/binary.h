#pragma once

#include "ast/node_base.h"
#include "gen/generator_if.h"

namespace ast {

	class Binary : public Node_base {
		public:
			Binary(Node_if& left, Node_if& right)
		 		:	Node_base(),
					m_left(left),
					m_right(right) {
			}

			virtual ~Binary() {
			}

			virtual void set_generator(gen::Generator_if& g) {
				Node_base::set_generator(g);
				m_left.set_generator(g);
				m_right.set_generator(g);
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
