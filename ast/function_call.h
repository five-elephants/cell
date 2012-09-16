#pragma once

#include "ast/node_base.h"
#include <vector>

namespace ast {

	class Function_call : public Node_base {
		public:
			Function_call(Node_if& identifier);
			virtual ~Function_call();

			virtual void visit();
			virtual void set_generator(gen::Generator_if& g);

			Node_if& identifier();
			void expressions(std::vector<Node_if*>& nodes);
			std::vector<Node_if*>& expressions();

		private:
			Node_if& m_identifier;
			std::vector<Node_if*> m_expressions;
	};

}
