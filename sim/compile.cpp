#include "sim/compile.h"

#include "ir/scan_ast.h"
#include "sim/llvm_codegen.h"

namespace sim {

  ir::Namespace compile(ast::Node_if const& ast_root, std::string const& defaultname) {
    ir::Namespace rv(defaultname);
    Llvm_codegen codegen;

    scan_ast(rv, ast_root, codegen);
    return rv;
  }

}

