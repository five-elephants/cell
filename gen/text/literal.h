#pragma once

#include "ast/literal.h"

#include <ostream>

namespace gen_text {

	template<typename T>
	std::ostream& operator<<(std::ostream& out, ast::Literal<T> const& lit) {
		return out << lit.value();
	}

}
