#pragma once

namespace ast {
	// forward declarations
	class Op_plus;
	class Op_minus;
	class Op_mult;
	class Op_div;
	template<typename T> class Literal;
	class Var_assign;
	class Identifier;
	class Module_def;
	class Function_def;
}

namespace gen {
	class Generator_if {
		public:
			// binary operators
			virtual void op_plus(ast::Op_plus& op) = 0;
			virtual void op_minus(ast::Op_minus& op) = 0;
			virtual void op_mult(ast::Op_mult& op) = 0;
			virtual void op_div(ast::Op_div& op) = 0;

			// literals
			virtual void int_literal(ast::Literal<int>& lit) = 0;

			// variables
			virtual void var_assign(ast::Var_assign& a) = 0;

			// identifiers
			virtual void identifier(ast::Identifier& id) = 0;

			// modules
			virtual void module_begin(ast::Module_def& mod) = 0;
			virtual void module_end(ast::Module_def& mod) = 0;

			// functions
			virtual void function_begin(ast::Function_def& f) = 0;
			virtual void function_body(ast::Function_def& f) = 0;
			virtual void function_end(ast::Function_def& f) = 0;
	};
}
