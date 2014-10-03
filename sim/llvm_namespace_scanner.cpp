#include "sim/llvm_namespace_scanner.h"

#include "sim/llvm_function_scanner.h"


namespace sim {

  Llvm_namespace_scanner::Llvm_namespace_scanner(sim::Llvm_namespace& ns)
    : ir::Namespace_scanner(ns) {
  }


  bool
  Llvm_namespace_scanner::insert_function(ast::Function_def const& node) {
    std::shared_ptr<ir::Function> func = create_function(node);
    
    m_ns.functions[func->name] = func;

    // code generation
    Llvm_function_scanner scanner(m_ns, *func);
    node.accept(scanner);

    return false;
  }

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
