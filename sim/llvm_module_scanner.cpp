#include "llvm_module_scanner.h"

#include "sim/llvm_function_scanner.h"


#include <iostream>


namespace sim {

  using Module_scanner = ir::Module_scanner<Llvm_impl>;


  Llvm_module_scanner::Llvm_module_scanner(Llvm_module& mod)
    : Module_scanner(mod) {
  } 


  bool
  Llvm_module_scanner::insert_function(ast::Function_def const& node) {
    std::cout << "inserting function" << std::endl;

    std::shared_ptr<Llvm_function> func = create_function(node);
    
    m_ns.functions[func->name] = func;

    // code generation
    Llvm_function_scanner scanner(m_ns, *func);
    node.accept(scanner);

    return false;
  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
