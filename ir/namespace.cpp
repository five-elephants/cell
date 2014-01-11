#include "namespace.h"

#include "ast/ast_find.h"
#include "namespace_scanner.h"
#include "module_scanner.h"
#include "builtins.h"
#include "find.hpp"
#include "types.h"
#include "streamop.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <set>
//--------------------------------------------------------------------------------

namespace ir {

  //--------------------------------------------------------------------------------
  Socket::Socket(Label name)
    : enclosing_ns(nullptr) {
    this->name = name;
  }

  Socket::Socket(ast::Socket_def const& sock)
    : enclosing_ns(nullptr) {
    name = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
  }


  void
  Socket::scan_ast(ast::Node_if const& tree) {
    auto port_nodes = ast::find_by_type<ast::Socket_item>(tree);
    
    for(auto i : port_nodes) {
      std::shared_ptr<Port> port(new Port);

      port->name = dynamic_cast<ast::Identifier const&>(i->name()).identifier();
      if( ports.count(port->name) > 0 ) {
        std::stringstream strm;
        strm << i->location();
        strm << ": socket already contains port of name '" << port->name << "'";
        throw std::runtime_error(strm.str());
      }

      switch(i->direction()) {
        case ast::Socket_input:
          port->direction = Direction::Input;
          break;

        case ast::Socket_output:
          port->direction = Direction::Output;
          break;

        case ast::Socket_bidirectional:
          port->direction = Direction::Bidirectional;
          break;

        default:
          throw std::runtime_error("Invalid data direction in AST");
      }

      auto type_name = dynamic_cast<ast::Identifier const&>(i->type()).identifier();
      // XXX only builtin types for now
      {
        auto it = Builtins::types.find(type_name);
        if( it != Builtins::types.end() )
          port->type = it->second;
        else {
          std::stringstream strm;
          strm << i->type().location();
          strm << ": typename '" << type_name << "' not found.";
          throw std::runtime_error(strm.str());
        }
      }

      ports[port->name] = port;
    }
  }
  //--------------------------------------------------------------------------------
  Module::Module(ast::Module_def const& mod)
    : Namespace(dynamic_cast<ast::Identifier const*>(&(mod.identifier()))->identifier()) {
  }


  void
  Module::scan_ast(ast::Node_if const& tree) {
    auto mod = dynamic_cast<ast::Module_def const&>(tree);
    if( mod.has_socket() ) {
      if( typeid(mod.socket()) == typeid(ast::Identifier) ) {
        auto socket_name = dynamic_cast<ast::Identifier const&>(mod.socket()).identifier();
        socket = find_socket(*this, socket_name);
        if( !socket ) {
          std::stringstream strm;
          strm << tree.location()
            << ": failed to find socket of name "
            << socket_name
            << " in module "
            << name;
          throw std::runtime_error(strm.str());
        }
      } else if( typeid(mod.socket()) == typeid(ast::Socket_def) ) {
        auto sock = dynamic_cast<ast::Socket_def const&>(mod.socket());
        socket = insert_socket(sock);
      }
    } else {
      socket = Builtins::null_socket;
    }

    Module_scanner scanner(*this);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------
  void
  Module::insert_object(ast::Variable_def const& node) {
    std::shared_ptr<Object> obj(new Object);
    obj->name = dynamic_cast<ast::Identifier const*>(&(node.identifier()))->identifier();
    if( objects.count(obj->name) > 0 )
      throw std::runtime_error(std::string("Variable with name ")
          + obj->name
          + std::string(" already exists"));

    auto type_name = dynamic_cast<ast::Identifier const*>(&(node.type()))->identifier();
    obj->type = find_type(*this, type_name);
    if( !obj->type ) {
      std::stringstream strm;
      strm << node.type().location();
      strm << ": typename '" << type_name << "' not found.";
      throw std::runtime_error(strm.str());
    }
    objects[obj->name] = obj;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_module(ast::Module_def const& mod) {
    auto m = std::shared_ptr<Module>(new Module(mod));
    if( modules.count(m->name) > 0 )
      throw std::runtime_error(std::string("Module with name ")+ m->name +std::string(" already exists"));

    m->enclosing_ns = this;
    m->scan_ast(mod);
    modules[m->name] = m;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_namespace(ast::Namespace_def const& ns) {
    auto label = dynamic_cast<ast::Identifier const*>(&(ns.identifier()))->identifier();
    auto n = std::shared_ptr<Namespace>(new Namespace(label));
    if( namespaces.count(n->name) > 0 )
      throw std::runtime_error(std::string("Namespace with name ")+ n->name +std::string(" already exists"));

    n->enclosing_ns = this;
    n->scan_ast(ns);
    namespaces[n->name] = n;
  }
  //--------------------------------------------------------------------------------
  std::shared_ptr<Socket>
  Namespace::insert_socket(ast::Socket_def const& sock) {
    auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
    auto s = std::shared_ptr<Socket>(new Socket(sock));
    if( sockets.count(s->name) > 0 )
      throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));
    if( types.count(s->name) > 0 )
      throw std::runtime_error(std::string("Type with name ") + s->name +std::string(" already exists"));

    s->enclosing_ns = this;
    s->scan_ast(sock);
    sockets[s->name] = s;
    types[s->name] = s;

    return s;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_function(ast::Function_def const& node) {
    std::shared_ptr<Function> func(new Function);
    func->name = dynamic_cast<ast::Identifier const*>(&(node.identifier()))->identifier();
    if( functions.count(func->name) > 0 )
      throw std::runtime_error(std::string("Function with name ")+ func->name +std::string(" already exists"));

    auto type_name = dynamic_cast<ast::Identifier const*>(&(node.return_type()))->identifier();
    func->return_type = find_type(*this, type_name);
    if( !func->return_type ) {
      std::stringstream strm;
      strm << node.return_type().location();
      strm << ": return type '" << type_name << "' not found.";
      throw std::runtime_error(strm.str());
    }
    functions[func->name] = func;
  } 
  //--------------------------------------------------------------------------------
  void
  Namespace::scan_ast(ast::Node_if const& tree) {
    Namespace_scanner scanner(*this);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------
  
}
