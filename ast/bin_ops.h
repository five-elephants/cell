#pragma once

#include "ast/binary.h"

#include <utility>

namespace ast {

#define BINOP(name) \
	class Op_ ## name : public Binary {  \
		public:  \
			Op_ ## name (Node_if& left, Node_if& right)  \
				:	Binary(left, right) {  \
			}  \
	\
			virtual void visit() {  \
			}  \
	}

	BINOP(plus);
	BINOP(minus);
	BINOP(mult);
	BINOP(div);
  BINOP(equal);
  BINOP(not_equal);
  BINOP(greater_then);
  BINOP(lesser_then);
  BINOP(greater_or_equal_then);
  BINOP(lesser_or_equal_then);
  BINOP(and);
  BINOP(or);

}

#undef BINOP
