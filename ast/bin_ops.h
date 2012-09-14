#pragma once

#include "ast/binary.h"

#include <utility>

namespace ast {

#define BINOP(name) \
	class Op_ ## name : public Binary {  \
		public:  \
			Op_ ## name (Node_if& left, Node_if& right, gen::Generator_if& g)  \
				:	Binary(left, right, g) {  \
			}  \
	\
			virtual void visit() {  \
				get_generator().op_ ## name(*this);  \
			}  \
	}

	BINOP(plus);
	BINOP(minus);
	BINOP(mult);
	BINOP(div);

}

#undef BINOP
