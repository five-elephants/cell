#include "analyze.h"

#include "scan_ast.h"

namespace ir {

  Namespace analyze(ast::Node_if const& ast_root, std::string const& defaultname) {
    Namespace rv(defaultname);
    Null_codegen codegen;

    scan_ast(rv, ast_root, codegen);
    return rv;
  }

}
