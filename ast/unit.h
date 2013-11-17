#pragma once

#include "ast/node_base.h"
#include <vector>

namespace ast {

	class Unit : public Node_base {
		public:
			Unit()
				:	Node_base() {
			}

			virtual ~Unit() {};

			virtual void set_generator(gen::Generator_if& g);
			virtual void visit();
      virtual void visit(std::function<void(Node_if const&)> cb) const;

			void append(Node_if& node);
			void append(std::vector<Node_if*> const& nodes);

		private:
			std::vector<Node_if*> m_elements;
	};

}
