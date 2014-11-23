#include "sim/compile.h"

#include "sim/scan_ast.h"
#include "sim/llvm_namespace_scanner.h"
#include "ast/ast_printer.h"

#include <memory>

namespace sim {

  sim::Llvm_library compile(ast::Node_if const& ast_root,
      std::string const& defaultname) {
    ir::Library<Llvm_impl> rv;

    rv.name = defaultname;
    rv.ns = std::make_shared<sim::Llvm_namespace>();

    ast::Ast_printer printer(std::cout);
    ast_root.accept(printer);

    Llvm_namespace_scanner scanner(*rv.ns, rv);
    ast_root.accept(scanner);

    return rv;
  }

}

