#include "sim/compile.h"

#include "ir/scan_ast.h"
#include "sim/llvm_codegen.h"

#include <memory>

namespace sim {

  std::tuple<ir::Namespace, std::shared_ptr<Llvm_codegen>> compile(ast::Node_if const& ast_root,
      std::string const& defaultname) {
    ir::Namespace rv(defaultname);
    auto codegen = std::make_shared<Llvm_codegen>();

    scan_ast(rv, ast_root, *codegen);
    codegen->emit();
    return std::make_tuple(rv, codegen);
  }

}

