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
      virtual void visit(std::function<void(Node_if const&)> cb) const;

			void append(Node_if& node);
			void append(std::vector<Node_if*> const& nodes);

			Node_if& identifier() { return m_identifier; }
			Node_if const& identifier() const { return m_identifier; }

		private:
			Node_if& m_identifier;
			std::vector<Node_if*> m_elements;
	};

}
