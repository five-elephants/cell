#pragma once

#include "ast/node_base.h"

namespace ast {
	
	class Empty_expression : public Node_base {
		public:
			Empty_expression() 
				:	Node_base() {
			}

			virtual void visit() {
			}
	};

}

