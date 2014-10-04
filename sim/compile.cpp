#include "sim/compile.h"

#include "sim/scan_ast.h"
#include "sim/llvm_namespace_scanner.h"

#include <memory>

namespace sim {

  sim::Llvm_library compile(ast::Node_if const& ast_root,
      std::string const& defaultname) {
    ir::Library<Llvm_impl> rv;

    Llvm_namespace_scanner scanner(*rv.ns, rv);
    ast_root.accept(scanner);

    return rv;
  }

}

