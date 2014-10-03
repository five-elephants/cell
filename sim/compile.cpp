#include "sim/compile.h"

#include "sim/scan_ast.h"

#include <memory>

namespace sim {

  sim::Llvm_namespace compile(ast::Node_if const& ast_root,
      std::string const& defaultname) {
    sim::Llvm_namespace rv(defaultname);

    scan_ast(rv, ast_root);
    return rv;
  }

}

