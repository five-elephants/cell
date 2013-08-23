#include "gen/gen_m4.h"

#include "ast/ast.h"
#include "gen/util.h"

	namespace gen {

#define GEN_CLASSNAME M4_generator

#define M4_ESCAPE(x) m4_escape(x) 

#define BINOP_OUT(x) \
		void \
		GEN_CLASSNAME ::op_ ## x (ast::Op_ ## x & op) { \
			m_out << #x << '('; \
			op.left().visit(); \
			m_out	<< ", "; \
			op.right().visit(); \
			m_out	<< ')'; \
		}


		BINOP_OUT(plus)
		BINOP_OUT(minus)
		BINOP_OUT(mult)
		BINOP_OUT(div)
		BINOP_OUT(equal)
		BINOP_OUT(not_equal)
		BINOP_OUT(greater_then)
		BINOP_OUT(lesser_then)
		BINOP_OUT(greater_or_equal_then)
		BINOP_OUT(lesser_or_equal_then)

#undef BINOP_OUT

		DEF_GENERATOR_FUNC(int_literal, Literal<int>, lit) {
			m_out << M4_ESCAPE(lit.value());
		}

		DEF_GENERATOR_FUNC(bitstring_literal, Bitstring_literal, lit) {
			m_out << "bitstring(" << M4_ESCAPE(lit.bitstring()) << ")";
		}

		DEF_GENERATOR_FUNC(variable_def, Variable_def, a) {
			//indent();

			if( within_function_params ) {
				if( a.without_expression() )
					m_out << "param_decl(";
				else
					m_out << "param_def(";
			} else {
				if( a.without_expression() )
					m_out << "variable_decl(";
				else
					m_out << "variable_def(";
			}

			a.type().visit();
			m_out << ", ";
			a.identifier().visit();

			if( !a.without_expression() ) {
				m_out << ", ";
				a.expression().visit();
			}
			m_out << ')';
		}


		DEF_GENERATOR_FUNC(identifier, Identifier, id) {
			m_out << M4_ESCAPE(id.identifier());
		}


		DEF_GENERATOR_FUNC(module_begin, Module_def, mod) {
			indent();
			m_out << "module(";
			mod.identifier().visit();
			m_out << ", ";
			++m_indent;
		}


		DEF_GENERATOR_FUNC(module_end, Module_def, mod) {
			--m_indent;
			indent();
			m_out << ")\n";
		}


		DEF_GENERATOR_FUNC(function_begin, Function_def, f) {
			indent();
			within_function_params = true;
			m_out << "function_def(";
			f.identifier().visit();
			m_out << ", type(`";
			f.return_type().visit();
			m_out << "'), `param_list(";
			++m_indent;
		}


		DEF_GENERATOR_FUNC(function_body, Function_def, f) {
			within_function_params = false;
			--m_indent;
			indent();
			m_out << ")', ";
			++m_indent;
		}


		DEF_GENERATOR_FUNC(function_end, Function_def, f) {
			--m_indent;
			indent();
			m_out << ")";
		}


		DEF_GENERATOR_FUNC(function_call, Function_call, f) {
			indent();
			m_out << "function_call(";
			f.identifier().visit();
			m_out << ", ";
			for(auto i : f.expressions()) {
				i->visit();
				m_out << ", ";
			}
			m_out << ")";
		}


		DEF_GENERATOR_FUNC(compound, Compound, c) {
			indent();
			m_out << "compound(";
			++m_indent;

			for(auto i=begin(c.statements()); i != end(c.statements());) {
				indent();
				(*i)->visit();
				if( ++i != end(c.statements()) )
					m_out << ", ";
			}
			//join_nodes_line(begin(c.statements()), end(c.statements()), *this);

			--m_indent;
			indent();
			m_out << ")";
		}


		DEF_GENERATOR_FUNC(if_statement, If_statement, i) {
			indent();
			if( i.has_else_body() )
				m_out << "if_else_statement(`";
			else
				m_out << "if_statement(`";

			i.condition().visit();
			m_out << "', `";
			++m_indent;
			i.body().visit();
			--m_indent;
			indent();
			
			if( i.has_else_body() ) {
				m_out << "', `";
				++m_indent;
				i.else_body().visit();
				--m_indent;
				indent();
			}
			m_out << "')";
		}

		DEF_GENERATOR_FUNC(while_statement, While_statement, w) {
			indent();
			m_out << "while_statement(`";
			w.expression().visit();
			m_out << "', `";
			++m_indent;
			w.body().visit();
			--m_indent;
			indent();
			m_out << "')";
		}

		void
		M4_generator::generic_statement(std::string const& name, std::vector<ast::Node_if*>& objs) {
			m_out << name << "(";
			join_nodes_comma(begin(objs), end(objs), *this);
			m_out << ")";
		}

		
		void
		M4_generator::comma_sep() {
			m_out << ", ";
		}


		void
		M4_generator::line_sep() {
			m_out << "\n";
  }

	void
	M4_generator::indent() const {
		//for(int i=0; i<m_indent; i++) {
			//m_out << "  ";
		//}
	}
  
}
