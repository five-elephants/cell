#include "scan_ast.h"

#include "llvm_namespace_scanner.h"

namespace sim {


	void scan_ast(sim::Llvm_namespace& ns, ast::Node_if const& tree) {
		Llvm_namespace_scanner scanner(ns);
		tree.accept(scanner);
	}


}


/* vim: set noet fenc= ff=unix sts=0 sw=2 ts=2 : */
