#include "gen/gen_cpp.h"

#include "ast/ast.h"

namespace gen {

#define BINOP_OUT(x, str) \
	void \
	Cpp_generator::op_ ## x (ast::Op_ ## x & op) { \
		m_out << '('; \
	 	op.left().visit(); \
	 	m_out	<< str; \
	 	op.right().visit(); \
		m_out	<< ')'; \
	}


  BINOP_OUT(plus, " + ")
  BINOP_OUT(minus, " - ")
  BINOP_OUT(mult, " * ")
  BINOP_OUT(div, " / ")
  BINOP_OUT(equal, " == ")
  BINOP_OUT(not_equal, " != ")
  BINOP_OUT(greater_then, " > ")
  BINOP_OUT(lesser_then, " < ")
  BINOP_OUT(greater_or_equal_then, " >= ")
  BINOP_OUT(lesser_or_equal_then, " <= ")

#undef BINOP_OUT

	void
	Cpp_generator::int_literal(ast::Literal<int>& lit) {
		m_out << lit.value();
	}

  void
  Cpp_generator::bitstring_literal(ast::Bitstring_literal& lit) {
    m_out << "\"" << lit.bitstring() << "\"";
  }


	void
	Cpp_generator::variable_def(ast::Variable_def& a) {
		indent();

    a.type().visit();
    m_out << ' ';
    a.identifier().visit();

    if( !a.without_expression() ) {
      m_out << " = ";
      a.expression().visit();
    }
    m_out << ";\n";
	}


	void
	Cpp_generator::identifier(ast::Identifier& id) {
		m_out << id.identifier();
	}


	void
	Cpp_generator::module_begin(ast::Module_def& mod) {
		indent();
    m_out << "class ";
    mod.identifier().visit();
    m_out << " {\n";
		++m_indent;
	}


	void
	Cpp_generator::module_end(ast::Module_def& mod) {
		--m_indent;
		indent();
		m_out << "};\n";
	}


	void
	Cpp_generator::function_begin(ast::Function_def& f) {
		indent();
    f.return_type().visit();
    m_out << ' ';
    f.identifier().visit();
    m_out << '(';
		++m_indent;
	}


	void
	Cpp_generator::function_body(ast::Function_def& f) {
		--m_indent;
		indent();
		m_out << ") {\n";
		++m_indent;
	}


	void
	Cpp_generator::function_end(ast::Function_def& f) {
		--m_indent;
		indent();
		m_out << "}\n";
	}


	void
	Cpp_generator::function_call(ast::Function_call& f) {
		indent();
		f.identifier().visit();
		m_out << "(";
		for(auto i : f.expressions()) {
			i->visit();
			m_out << ", ";
		}
		m_out << ")";
	}


	void
	Cpp_generator::compound(ast::Compound& c) {
		indent();
		m_out << "{\n";
		++m_indent;

		for(auto i : c.statements()) {
			i->visit();
			m_out << "\n";
		}

		--m_indent;
		indent();
		m_out << "}\n";
	}


	void
	Cpp_generator::if_statement(ast::If_statement& i) {
		indent();
		m_out << "if( ";
		i.condition().visit();
		m_out << " )\n";
		++m_indent;
		i.body().visit();
		--m_indent;
		indent();
    
    if( i.has_else_body() ) {
      m_out << " else\n";
      ++m_indent;
      i.else_body().visit();
      m_out << "\n";
      --m_indent;
      indent();
    } else {
		  m_out << "\n";
      indent();
    }
	}

  void
  Cpp_generator::while_statement(ast::While_statement& w) {
    indent();
    m_out << "while(";
    w.expression().visit();
    m_out << ")\n";
    ++m_indent;
    w.body().visit();
    --m_indent;
    indent();
    m_out << "\n";
  }

	void
	Cpp_generator::indent() const {
		for(int i=0; i<m_indent; i++) {
			m_out << "  ";
		}
	}


}
