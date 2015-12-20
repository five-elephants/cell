#include "sim/llvm_namespace_scanner.h"

#include "sim/llvm_module_scanner.h"
#include "sim/llvm_function_scanner.h"
#include "sim/llvm_constexpr_scanner.h"
#include "sim/socket_operator_codegen.h"
#include "ir/path.h"
#include "ir/builtins.h"
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
    auto m = create_module(mod);
    LOG4CXX_TRACE(m_logger, "Llvm_namespace_scanner::insert_module for '"
        << m->name << "'");

    Llvm_module_scanner scanner(*m);
    mod.accept(scanner);
    m_ns.modules[m->name] = m;

    return false;
  }

  bool
  Llvm_namespace_scanner::insert_function(ast::Function_def const& node) {
    std::shared_ptr<Llvm_function> func = create_function(node);

    m_ns.functions.insert(std::make_pair(func->name, func));

    // code generation
    Llvm_function_scanner scanner(m_ns, *func);
    node.accept(scanner);

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


  bool
  Llvm_namespace_scanner::insert_table(ast::Table_def const& node) {
    auto ty = create_table_type(node);
    m_ns.types[ty.first->name] = ty.first;
    m_ns.namespaces[ty.second->name] = ty.second;

    auto cnst_it = ty.first->allowed_values.begin();
    for(size_t i=0; i<ty.first->allowed_values.size(); ++i) {
      auto cnst = (cnst_it++)->second;
      auto item = node.items().at(i);

      LOG4CXX_TRACE(m_logger, "creating constant for "
          << ty.first->name << "::" << cnst->name);

      Llvm_constexpr_scanner scanner(cnst, m_ns);
      item.second->accept(scanner);
    }

    // create operators
    auto base_op_eq = ir::find_operator(m_ns, "==",
        ir::Builtins<Llvm_impl>::types.at("bool"),
        ty.first->array_base_type,
        ty.first->array_base_type);
    if( !base_op_eq ) {
      std::stringstream strm;
      strm << node.location() << ": base type '"
        << ty.first->array_base_type->name
        << "' does not have equality operator '=='";
      throw std::runtime_error(strm.str());
    }

    auto base_op_neq = ir::find_operator(m_ns, "!=",
        ir::Builtins<Llvm_impl>::types.at("bool"),
        ty.first->array_base_type,
        ty.first->array_base_type);
    if( !base_op_neq ) {
      std::stringstream strm;
      strm << node.location() << ": base type '"
        << ty.first->array_base_type->name
        << "' does not have inequality operator '!='";
      throw std::runtime_error(strm.str());
    }

    // equality comparison
    auto op_eq = std::make_shared<Llvm_operator>();
    op_eq->name = "==";
    op_eq->return_type = ir::Builtins<Llvm_impl>::types.at("bool");
    op_eq->left = op_eq->right = ty.first;
    op_eq->impl.insert_func = base_op_eq->impl.insert_func;
    op_eq->impl.const_insert_func = base_op_eq->impl.const_insert_func;
    m_ns.operators.insert(std::make_pair(op_eq->name, op_eq));

    // inequality operator
    auto op_neq = std::make_shared<Llvm_operator>();
    op_neq->name = "!=";
    op_neq->return_type = ir::Builtins<Llvm_impl>::types.at("bool");
    op_neq->left = op_neq->right = ty.first;
    op_neq->impl.insert_func = base_op_neq->impl.insert_func;
    op_neq->impl.const_insert_func = base_op_neq->impl.const_insert_func;
    m_ns.operators.insert(std::make_pair(op_neq->name, op_neq));

    // conversion to base type
    auto op_conv = std::make_shared<Llvm_operator>();
    op_conv->name = "convert";
    op_conv->return_type = ty.first->array_base_type;
    op_conv->left = ty.first;
    op_conv->right = ty.first;
    op_conv->impl.insert_func = [](llvm::IRBuilder<> bld,
        llvm::Value* left,
        llvm::Value* right) -> llvm::Value* {
      return left;
    };
    op_conv->impl.const_insert_func = [](llvm::Constant* left,
        llvm::Constant* right) -> llvm::Constant* {
      return left;
    };
    m_ns.operators.insert(std::make_pair(op_conv->name, op_conv));

    // conversion from base type
    op_conv = std::make_shared<Llvm_operator>();
    op_conv->name = "convert";
    op_conv->return_type = ty.first;
    op_conv->left = ty.first->array_base_type;
    op_conv->right = ty.first->array_base_type;
    op_conv->impl.insert_func = [](llvm::IRBuilder<> bld,
        llvm::Value* left,
        llvm::Value* right) -> llvm::Value* {
      return left;
    };
    op_conv->impl.const_insert_func = [](llvm::Constant* left,
        llvm::Constant* right) -> llvm::Constant* {
      return left;
    };
    m_ns.operators.insert(std::make_pair(op_conv->name, op_conv));

    return false;
  }


  std::shared_ptr<Llvm_type>
  Llvm_namespace_scanner::create_array_type(ast::Array_type const& node) {
    // process constant expression to determine size
    auto sz_cnst = std::make_shared<Llvm_constant>();
    Llvm_constexpr_scanner scanner(sz_cnst, m_ns);
    node.size_expr().accept(scanner);

    // get size from constant expression
    if( sz_cnst->type != ir::Builtins<Llvm_impl>::types["int"] ) {
      std::stringstream strm;
      strm << node.location()
        << ": Constant for array size is not of type 'int'"
        << " (is of type '"
        << sz_cnst->type->name
        << "')";
      throw std::runtime_error(strm.str());
    }

    auto sz = sz_cnst->impl.expr->getUniqueInteger().getLimitedValue();


    if( typeid(node.base_type()) == typeid(ast::Array_type) ) {
      auto& base_type = dynamic_cast<ast::Array_type const&>(node.base_type());
      auto bt = this->create_array_type(base_type);

      std::stringstream strm;
      strm << bt->name << "[" << sz << "]";
      auto new_type = std::make_shared<Llvm_type>();
      new_type->name = strm.str();
      new_type->array_size = sz;

      return new_type;
    } else if( typeid(node.base_type()) == typeid(ast::Identifier) ) {
      auto& base_type = dynamic_cast<ast::Identifier const&>(node.base_type());
      auto type_name = base_type.identifier();
      auto ir_type = find_type(m_ns, type_name);

      if( !ir_type ) {
        std::stringstream strm;
        strm << node.location()
          << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }

      std::stringstream strm;
      strm << ir_type->name << "[" << sz << "]";
      auto new_type = std::make_shared<Llvm_type>();
      new_type->name = strm.str();
      new_type->array_size = sz;

      return new_type;
    } else {
      throw std::runtime_error("Array base type is neither array nor identifier.");
    }
  }


}


/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
