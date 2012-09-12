#pragma once

#include "node_if.h"

namespace ast {
	
	template<typename T>
	class Literal : public Node_if {
		public:
			Literal(T const& value)
				:	m_value(value) {
			}

		T const& value() const { return m_value; }

		private:
			T m_value;
	};

}
