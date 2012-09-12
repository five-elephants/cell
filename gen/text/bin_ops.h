#include "ast/bin_ops.h"

#include <string>
#include <ostream>


namespace gen_text {

#define OPER(T, txt) \
	template<typename Left, typename Right> \
	std::ostream& operator<<(std::ostream& out, T<Left,Right>& op) {  \
		return out << '(' << op.left() << txt << op.right() << ')';        \
	}

	OPER(ast::Op_plus, "+")
	OPER(ast::Op_minus, "-")

#undef OPER

}
