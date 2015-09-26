#pragma once

#include <string>

#include "namespace.h"
#include "ast/node_if.h"

namespace ir {

  template<typename Impl = No_impl>
  Namespace<Impl> analyze(ast::Node_if const& ast_root,
      std::string const& defaultname = "default") {
    Namespace<Impl> rv(defaultname);

    scan_ast(rv, ast_root);
    return rv;
  }

}
