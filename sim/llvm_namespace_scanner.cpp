#include "sim/llvm_namespace_scanner.h"

#include "sim/llvm_module_scanner.h"
#include "sim/llvm_function_scanner.h"

#include <iostream>


namespace sim {

  //using Namespace_scanner = ir::Namespace_scanner<Llvm_impl>;


  bool say_hello() {
    std::cout << "hello" << std::endl;
    return true;
  }


  Llvm_namespace_scanner::Llvm_namespace_scanner(Llvm_namespace& ns)
    : ir::Namespace_scanner<Llvm_impl>(ns) {
  }


  bool
  Llvm_namespace_scanner::insert_namespace(ast::Namespace_def const& ns) {
    std::cout << "Llvm_namespace_scanner::insert_namespace" << std::endl;

    auto n = create_namespace(ns);

    Llvm_namespace_scanner scanner(*n);
    ns.accept(scanner);

    m_ns.namespaces[n->name] = n;

    return false;
  }


  bool
  Llvm_namespace_scanner::insert_module(ast::Module_def const& mod) {
    std::cout << "Llvm_namespace_scanner::insert_module" << std::endl;
    auto m = create_module(mod);

    Llvm_module_scanner scanner(*m);
    mod.accept(scanner);
    m_ns.modules[m->name] = m;

    return false;
  }

  bool
  Llvm_namespace_scanner::insert_function(ast::Function_def const& node) {
    std::shared_ptr<Llvm_function> func = create_function(node);
    
    m_ns.functions[func->name] = func;

    // code generation
    Llvm_function_scanner scanner(m_ns, *func);
    node.accept(scanner);

    // (XXX unsure if this is needed) optimize code
    //auto lib = ir::find_library(m_ns);
    //lib->impl.fpm->run(*(func->impl.code));

    return false;
  }

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
