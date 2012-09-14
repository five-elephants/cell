#pragma once

#include "ast/node_base.h"
#include "gen/generator_if.h"
#include <vector>

namespace ast {

	class Function_def : public Node_base {
		public:
			Function_def(Node_if& identifier);
			virtual ~Function_def() {}

			virtual void set_generator(gen::Generator_if& g);
			virtual void visit();

			void append_parameter(Node_if& node);
			void append_body(Node_if& node);

			Node_if& identifier() { return m_identifier; }

		private:
			Node_if& m_identifier;
			std::vector<Node_if*> m_parameters;
			std::vector<Node_if*> m_body;
	};

}
