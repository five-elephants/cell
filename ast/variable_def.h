#pragma once

#include "ast/node_base.h"

namespace ast {

	class Variable_def : public Node_base {
		public:
			Variable_def(Node_if& identifier,
					Node_if& type,
					Node_if& expression);
			virtual ~Variable_def() {}

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

