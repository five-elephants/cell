#pragma once

#include "ast/tree_base.h"

namespace ast {

	class Unary : public Tree_base {
		public:
			Unary(Node_if& operand)
		 		:	Tree_base(),
					m_operand(operand) {
				register_branches({&m_operand});
			}

			Node_if& operand() { return m_operand; }
			Node_if const& operand() const { return m_operand; }

		private:
			Node_if& m_operand;
	};


}


/* vim: set noet fenc= ff=unix sts=0 sw=2 ts=2 : */
