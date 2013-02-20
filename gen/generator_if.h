#pragma once

namespace ast {
	// forward declarations
	class Op_plus;
	class Op_minus;
	class Op_mult;
	class Op_div;
  class Op_equal;
  class Op_not_equal;
  class Op_greater_then;
  class Op_lesser_then;
  class Op_greater_or_equal_then;
  class Op_lesser_or_equal_then;
	template<typename T> class Literal;
  class Bitstring_literal;
	class Variable_def;
	class Identifier;
	class Module_def;
	class Function_def;
	class Function_call;
	class Compound;
	class If_statement;
  class While_statement;
}

namespace gen {
	class Generator_if {
		public:
			// binary operators
			virtual void op_plus(ast::Op_plus& op) = 0;
			virtual void op_minus(ast::Op_minus& op) = 0;
			virtual void op_mult(ast::Op_mult& op) = 0;
			virtual void op_div(ast::Op_div& op) = 0;
      virtual void op_equal(ast::Op_equal& op) = 0;
      virtual void op_not_equal(ast::Op_not_equal& op) = 0;
      virtual void op_greater_then(ast::Op_greater_then& op) = 0;
      virtual void op_lesser_then(ast::Op_lesser_then& op) = 0;
      virtual void op_greater_or_equal_then(ast::Op_greater_or_equal_then& op) = 0;
      virtual void op_lesser_or_equal_then(ast::Op_lesser_or_equal_then& op) = 0;

			// literals
			virtual void int_literal(ast::Literal<int>& lit) = 0;
      virtual void bitstring_literal(ast::Bitstring_literal& lit) = 0;

			// variables
			virtual void variable_def(ast::Variable_def& a) = 0;

			// identifiers
			virtual void identifier(ast::Identifier& id) = 0;

			// modules
			virtual void module_begin(ast::Module_def& mod) = 0;
			virtual void module_end(ast::Module_def& mod) = 0;

			// functions
			virtual void function_begin(ast::Function_def& f) = 0;
			virtual void function_body(ast::Function_def& f) = 0;
			virtual void function_end(ast::Function_def& f) = 0;
			virtual void function_call(ast::Function_call& f) = 0;

			// statements
			virtual void compound(ast::Compound& c) = 0;
			virtual void if_statement(ast::If_statement& i) = 0;
      virtual void while_statement(ast::While_statement& i) = 0;

      // list separators
      virtual void comma_sep() = 0;
      virtual void line_sep() = 0;
	};
}
