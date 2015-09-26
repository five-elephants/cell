#pragma once

#include "ast/unary.h"


namespace ast {

#define UNAOP(name) \
	class Op_ ## name : public Unary {  \
		public:  \
			Op_ ## name (Node_if& operand)  \
				:	Unary(operand) {  \
			}  \
	}


  UNAOP(not);
  UNAOP(at);

}

#undef UNAOP
