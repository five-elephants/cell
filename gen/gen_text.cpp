#include "gen/gen_text.h"

#include "ast/ast.h"

namespace gen {

	void
	Text_generator::op_plus(ast::Op_plus& op) {
		m_out << '(';
	 	op.left().visit();
	 	m_out	<< " + ";
	 	op.right().visit();
		m_out	<< ')';
	}

	void
	Text_generator::op_minus(ast::Op_minus& op) {
		m_out << '(';
	 	op.left().visit();
	 	m_out	<< " - ";
	 	op.right().visit();
		m_out	<< ')';
	}

	void
	Text_generator::op_mult(ast::Op_mult& op) {
		m_out << '(';
		op.left().visit();
		m_out << " * ";
		op.right().visit();
		m_out << ')';
	}

	void
	Text_generator::op_div(ast::Op_div& op) {
		m_out << '(';
		op.left().visit();
		m_out << " / ";
		op.right().visit();
		m_out << ')';
	}
		

	void
	Text_generator::int_literal(ast::Literal<int>& lit) {
		m_out << lit.value();
	}


	void
	Text_generator::variable_def(ast::Variable_def& a) {
		indent();
		m_out << "(var ";
		a.identifier().visit();
		m_out << " : ";
		a.type().visit();
		if( !a.without_expression() ) {
			m_out << " = ";
			a.expression().visit();
		}
		m_out << ")\n";
	}


	void
	Text_generator::identifier(ast::Identifier& id) {
		m_out << id.identifier();
	}


	void
	Text_generator::module_begin(ast::Module_def& mod) {
		indent();
		m_out << "(mod ";
		mod.identifier().visit();
		m_out << " (\n";
		++m_indent;
	}


	void
	Text_generator::module_end(ast::Module_def& mod) {
		--m_indent;
		indent();
		m_out << ")\n";
	}


	void
	Text_generator::function_begin(ast::Function_def& f) {
		indent();
		m_out << "(function ";
		f.identifier().visit();
		m_out << " return_type ";
		f.return_type().visit();
		m_out << " (parameters (\n";
		++m_indent;
	}


	void
	Text_generator::function_body(ast::Function_def& f) {
		--m_indent;
		indent();
		m_out << ")) (body (\n";
		++m_indent;
	}


	void
	Text_generator::function_end(ast::Function_def& f) {
		--m_indent;
		indent();
		m_out << "))\n";
	}


	void
	Text_generator::function_call(ast::Function_call& f) {
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
	Text_generator::indent() const {
		for(int i=0; i<m_indent; i++) {
			m_out << "  ";
		}
	}


}
