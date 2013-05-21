#pragma once


#define DECL_GENERATOR_CLASS(name) \
	class name : public Generator_if 

#define GENERATOR_CLASS_BODY(name)  \
		public:  \
			name (std::ostream& out)  \
				:	m_out(out) {  \
					m_indent = 0;  \
				  m_out << OUTFILE_HEADER ;	\
			}  \
			virtual ~name() {};  \
  \
			virtual void op_plus(ast::Op_plus& op);  \
			virtual void op_minus(ast::Op_minus& op);  \
			virtual void op_mult(ast::Op_mult& op);  \
			virtual void op_div(ast::Op_div& op);  \
      virtual void op_equal(ast::Op_equal& op);  \
      virtual void op_not_equal(ast::Op_not_equal& op);  \
      virtual void op_greater_then(ast::Op_greater_then& op);  \
      virtual void op_lesser_then(ast::Op_lesser_then& op);  \
      virtual void op_greater_or_equal_then(ast::Op_greater_or_equal_then& op);  \
      virtual void op_lesser_or_equal_then(ast::Op_lesser_or_equal_then& op);  \
  \
			virtual void int_literal(ast::Literal<int>& lit);  \
      virtual void bitstring_literal(ast::Bitstring_literal& lit);  \
			virtual void variable_def(ast::Variable_def& a);  \
			virtual void identifier(ast::Identifier& id);  \
			virtual void module_begin(ast::Module_def& mod);  \
			virtual void module_end(ast::Module_def& mod);  \
			virtual void function_begin(ast::Function_def& f);  \
			virtual void function_body(ast::Function_def& f);  \
			virtual void function_end(ast::Function_def& f);  \
			virtual void function_call(ast::Function_call& f);  \
			virtual void compound(ast::Compound& c);  \
			virtual void if_statement(ast::If_statement& i);  \
      virtual void while_statement(ast::While_statement& w);  \
			virtual void generic_statement(std::string const& name, std::vector<ast::Node_if*>& objs); \
  \
      virtual void comma_sep();  \
      virtual void line_sep();  \
  \
		private:  \
			std::ostream& m_out;  \
			int m_indent;  \
  \
			void indent() const;

#define DEF_GENERATOR_FUNC(name, type, opname) \
	void \
	GEN_CLASSNAME :: name(ast:: type & opname)
