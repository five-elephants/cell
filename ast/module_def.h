#pragma once

#include "ast/node_base.h"
#include <vector>

namespace ast {

	class Module_def : public Node_base {
		public:
			Module_def(Node_if& identifier)
				:	Node_base(),
		 			m_identifier(identifier) {
			}

			virtual ~Module_def() {};

			virtual void set_generator(gen::Generator_if& g);
			virtual void visit();

			void append(Node_if& node);

			Node_if& identifier() { return m_identifier; }

		private:
			Node_if& m_identifier;
			std::vector<Node_if*> m_elements;
	};

}
