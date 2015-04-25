#include "sim/llvm_namespace_scanner.h"

#include "sim/llvm_module_scanner.h"
#include "sim/llvm_function_scanner.h"
#include "sim/llvm_constexpr_scanner.h"
#include "sim/socket_operator_codegen.h"
#include "ir/path.h"
#include "parse_driver.h"

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
    LOG4CXX_TRACE(m_logger, "Llvm_namespace_scanner::insert_namespace");

    auto n = create_namespace(ns);
    Llvm_namespace_scanner scanner(*n);

    if( ns.empty() ) {
      // try to parse namespace from another file
      auto filename = ir::path_lookup(m_ns.enclosing_library.lock(), n->name);
      if( filename.empty() ) {
        std::stringstream strm;
        strm << ns.location() << ": "
          << "Could not find file for namespace reference '"
          << n->name
          << "'";
        throw std::runtime_error(strm.str());
      }
      Parse_driver driver;
      if( driver.parse(filename) )
        throw std::runtime_error("Parse failed");

      driver.ast_root().accept(scanner);
    } else {
      ns.accept(scanner);
    }

    m_ns.namespaces[n->name] = n;

    return false;
  }


  bool
  Llvm_namespace_scanner::insert_module(ast::Module_def const& mod) {
    LOG4CXX_TRACE(m_logger, "Llvm_namespace_scanner::insert_module");
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


  bool
  Llvm_namespace_scanner::insert_constant(ast::Constant_def const& node) {
    auto c = create_constant(node);
    m_ns.constants[c->name] = c;

    Llvm_constexpr_scanner scanner(c, m_ns);
    node.accept(scanner);

    return false;
  }

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
