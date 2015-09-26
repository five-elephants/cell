#pragma once

#include "ast/node_base.h"

#include <stdexcept>
#include <string>

namespace ast {

	template<typename T>
	class Literal_base : public Node_base {
		public:
			Literal_base(T const& value)
				:	Node_base(),
					m_value(value) {
			}

		T const& value() const { return m_value; }

		private:
			T m_value;
	};


	template<typename T>
	class Literal : public Literal_base<T> {
		public:
			Literal(T const& value)
				:	Literal_base<T>(value) {
			}
	};


}
