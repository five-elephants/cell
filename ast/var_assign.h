#pragma once

#include "ast/node_base.h"

namespace ast {

	class Var_assign : public Node_base {
		public:
			Var_assign(Node_if& identifier,
					Node_if& type,
					Node_if& expression,
					gen::Generator_if& generator);
			virtual ~Var_assign() {}

			virtual void set_generator(gen::Generator_if& g); 
			virtual void visit();

			Node_if& identifier() { return m_identifier; }
			Node_if& type() { return m_type; }
			Node_if& expression() { return m_expression; }

		private:
			Node_if& m_identifier;
			Node_if& m_type;
			Node_if& m_expression;

	};

}

