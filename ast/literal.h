#pragma once

#include "ast/node_base.h"
#include "gen/generator_if.h"

#include <stdexcept>

namespace ast {
	
	template<typename T>
	class Literal_base : public Node_base {
		public:
			Literal_base(T const& value, gen::Generator_if& generator)
				:	Node_base(generator),
					m_value(value) {
			}
			virtual ~Literal_base() {};

		T const& value() const { return m_value; }

		private:
			T m_value;
	};

	
	template<typename T>
	class Literal : public Literal_base<T> {
		public:
			Literal(T const& value, gen::Generator_if& generator)
				:	Literal_base<T>(value, generator) {
			}
			virtual ~Literal() {};

			virtual void visit() {
				throw std::runtime_error("not implemented!");
			}
	};

	template<>
	class Literal<int> : public Literal_base<int> {
		public:
			Literal<int>(int const& value, gen::Generator_if& generator)
				:	Literal_base<int>(value, generator) {
			}
			virtual ~Literal<int>() {};

			virtual void visit() {
				get_generator().int_literal(*this);
			}
	};


}
