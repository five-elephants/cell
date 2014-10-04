#include "sim/llvm_namespace_scanner.h"

#include "sim/llvm_function_scanner.h"


namespace sim {

  using Namespace_scanner = ir::Namespace_scanner<Llvm_impl>;


  Llvm_namespace_scanner::Llvm_namespace_scanner(Llvm_namespace& ns, Llvm_library& lib)
    : Namespace_scanner(ns),
      m_library(lib),
      m_llvm_ns(ns) {
  }


  bool
  Llvm_namespace_scanner::insert_function(ast::Function_def const& node) {
    std::shared_ptr<Llvm_function> func = create_function(node);
    
    m_ns.functions[func->name] = func;

    // code generation
    Llvm_function_scanner scanner(m_llvm_ns, *func);
    node.accept(scanner);

    return false;
  }

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
