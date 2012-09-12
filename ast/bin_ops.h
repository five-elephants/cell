#pragma once

#include "ast/binary.h"

#include <utility>

namespace ast {

	template<typename Left, typename Right>
	class Op_plus : public Binary<Left,Right> {
		public:
			Op_plus(Left& left, Right& right)
				:	Binary<Left,Right>(left, right) {
			}
	};

	template<typename Left, typename Right>
	Op_plus<Left,Right> make_op_plus(Left&& left, Right&& right) {
		return Op_plus<Left,Right>(left, right);
	}
	
	template<typename Left, typename Right>
	class Op_minus : public Binary<Left,Right> {
		public:
			Op_minus(Left& left, Right& right)
				:	Binary<Left,Right>(left, right) {
			}
	};

	template<typename Left, typename Right>
	Op_minus<Left,Right> make_op_minus(Left&& left, Right&& right) {
		return Op_minus<Left,Right>(left, right);
	}

}
