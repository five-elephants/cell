#pragma once

#include "namespace.h"
#include "ast/node_if.h"
#include "ir/codegen.h"

namespace ir {

  void scan_ast(Socket& socket, ast::Node_if const& tree, Codegen_if& codegen);
  void scan_ast(Namespace& ns, ast::Node_if const& tree, Codegen_if& codegen);
  void scan_ast(Module& mod, ast::Node_if const& tree, Codegen_if& codegen);

}
