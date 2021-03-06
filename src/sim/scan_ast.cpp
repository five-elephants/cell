#include "scan_ast.h"

#include "llvm_namespace_scanner.h"

namespace sim {


	void scan_ast(Llvm_library& lib, sim::Llvm_namespace& ns, ast::Node_if const& tree) {
		Llvm_namespace_scanner scanner(ns, lib);
		tree.accept(scanner);
	}


}


/* vim: set noet ff=unix sts=0 sw=2 ts=2 : */
