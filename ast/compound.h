#pragma once

#include "ast/node_base.h"
#include <vector>

namespace ast {

	class Compound : public Node_base {
		public:
			Compound();
			virtual ~Compound();

			virtual void visit();
	    virtual void visit(std::function<void(Node_if const&)> cb) const;
			virtual void set_generator(gen::Generator_if& g);

			void statements(std::vector<Node_if*> const& nodes);
			std::vector<Node_if*>& statements();

		private:
			std::vector<Node_if*> m_statements;
	};

}
