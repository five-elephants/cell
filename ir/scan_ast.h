#pragma once

#include "namespace.h"

namespace ir {

  void scan_ast(Socket& socket, ast::Node_if const& tree);
  void scan_ast(Namespace& ns, ast::Node_if const& tree);
  void scan_ast(Module& mod, ast::Node_if const& tree);

}
