#pragma once

#include "ast/node_base.h"
#include <vector>

namespace ast {

	class If_statement : public Node_base {
		public:
			If_statement(Node_if& condition, Node_if& body);
			virtual ~If_statement();

			virtual void visit();
			virtual void set_generator(gen::Generator_if& g);

			Node_if& condition() { return m_condition; }
			Node_if& body() { return m_body; }

		private:
			Node_if& m_condition;
			Node_if& m_body;
	};

}
