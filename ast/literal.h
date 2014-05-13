#pragma once

#include "ast/node_base.h"
#include "gen/generator_if.h"

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

			virtual void visit() {
				throw std::runtime_error("not implemented!");
			}
	};

	template<>
	class Literal<int> : public Literal_base<int> {
		public:
			Literal<int>(int const& value)
				:	Literal_base<int>(value) {
			}

			virtual void visit() {
				get_generator().int_literal(*this);
			}
	};


	template<>
	class Literal<std::string> : public Literal_base<std::string> {
		public:
			Literal<std::string>(std::string const& value)
				:	Literal_base<std::string>(value) {
			}

			virtual void visit() {}
	};

}
