#pragma once

#include "ast/tree_base.h"
#include "gen/generator_if.h"

namespace ast {

	class Unary : public Tree_base {
		public:
			Unary(Node_if& operand)
		 		:	Tree_base(),
					m_operand(operand) {
				register_branches({&m_operand});
			}

			virtual void set_generator(gen::Generator_if& g) {
				Node_base::set_generator(g);
				m_operand.set_generator(g);
			}

			Node_if& operand() { return m_operand; }
			Node_if const& operand() const { return m_operand; }

		private:
			Node_if& m_operand;
	};


}


/* vim: set noet fenc= ff=unix sts=0 sw=2 ts=2 : */
