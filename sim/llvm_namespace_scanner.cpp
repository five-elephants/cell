#include "sim/llvm_namespace_scanner.h"

#include "sim/llvm_module_scanner.h"
#include "sim/llvm_function_scanner.h"
#include "sim/socket_operator_codegen.h"

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


  bool
  Llvm_namespace_scanner::insert_socket(ast::Socket_def const& node) {
    auto sock = create_socket(node);
    auto lib = find_library(m_ns);
    auto hier_name = hierarchical_name(m_ns, sock->name);
    auto ty = llvm::StructType::create(lib->impl.context, hier_name);

    std::size_t i = 0;
    std::vector<llvm::Type*> port_tys;
    port_tys.reserve(sock->elements.size());
    for(auto p : sock->elements) {
      p.second->impl.struct_index = i++;
      port_tys.push_back(p.second->type->impl.type);
    }

    ty->setBody(port_tys);
    sock->Type::impl.type = ty;
    LOG4CXX_DEBUG(m_logger, "created socket definition for '" << sock->name << "'");

    // generate code for operator
    auto op_left = find_operator(m_ns, "<<", sock, sock);
    if( !op_left )
      throw std::runtime_error("failed to find operator << for socket");

    auto op_right = find_operator(m_ns, ">>", sock, sock);
    if( !op_right )
      throw std::runtime_error("failed to find operator >> for socket");

    sock->impl.opgen_left = std::make_shared<Socket_operator_codegen>(sock,
        ir::Direction::Input);
    sock->impl.opgen_right = std::make_shared<Socket_operator_codegen>(sock,
        ir::Direction::Output);

    op_left->impl.insert_func = *(sock->impl.opgen_left);
    op_right->impl.insert_func = *(sock->impl.opgen_right);

    return false;
  }

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
