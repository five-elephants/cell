#pragma once

#include "llvm_namespace.h"
#include "ast/node_if.h"

namespace sim {

  void scan_ast(sim::Llvm_namespace& ns, ast::Node_if const& tree);

}

/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
