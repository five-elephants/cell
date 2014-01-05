#include "namespace.h"

#include "ast/ast_find.h"
#include "namespace_scanner.h"
#include "builtins.h"

#include <stdexcept>
#include <sstream>
//--------------------------------------------------------------------------------

namespace ir {

  //--------------------------------------------------------------------------------
  Socket::Socket(ast::Socket_def const& sock) {
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
  Module::Module(ast::Module_def const& mod) {
    name = dynamic_cast<ast::Identifier const*>(&(mod.identifier()))->identifier();
  }


  void
  Module::scan_ast(ast::Node_if const& tree) {
    // find declared variables
    auto vars = ast::find_by_type<ast::Variable_def>(tree);
    for(auto i : vars) {
      std::shared_ptr<Object> obj(new Object);
      obj->name = dynamic_cast<ast::Identifier const*>(&(i->identifier()))->identifier();
      auto type_name = dynamic_cast<ast::Identifier const*>(&(i->type()))->identifier();
      obj->type = find_type(*this, type_name);
      if( !obj->type ) {
        std::stringstream strm;
        strm << i->type().location();
        strm << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }
      objects[obj->name] = obj;
    }

    // find defined functions 
    auto funcs = ast::find_by_type<ast::Function_def>(tree);
    for(auto i : funcs) {
      std::shared_ptr<Function> func(new Function);
      func->name = dynamic_cast<ast::Identifier const*>(&(i->identifier()))->identifier();
      auto type_name = dynamic_cast<ast::Identifier const*>(&(i->return_type()))->identifier();
      func->return_type = find_type(*this, type_name);
      if( !func->return_type ) {
        std::stringstream strm;
        strm << i->return_type().location();
        strm << ": return type '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }
      functions[func->name] = func;
    }
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_module(ast::Module_def const& mod) {
    auto m = std::shared_ptr<Module>(new Module(mod));
    if( modules.count(m->name) > 0 )
      throw std::runtime_error(std::string("Module with name ")+ m->name +std::string(" already exists"));

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

    n->scan_ast(ns);
    namespaces[n->name] = n;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_socket(ast::Socket_def const& sock) {
    auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
    auto s = std::shared_ptr<Socket>(new Socket(sock));
    if( sockets.count(s->name) > 0 )
      throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));

    s->scan_ast(sock);
    sockets[s->name] = s;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::scan_ast(ast::Node_if const& tree) {
    Namespace_scanner scanner(*this);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------
  /** Find a type by name in the hierarchy
   * */
  std::shared_ptr<Type> find_type(Module const& m, Label type_name) {
    // search module types
    {
      auto it = m.types.find(type_name);
      if( it != m.types.end() ) 
        return it->second;
    }

    // search builtin types
    {
      auto it = Builtins::types.find(type_name);
      if( it != Builtins::types.end() )
        return it->second;
    }
    
    // not found
    return std::shared_ptr<Type>(nullptr);
  }
  //--------------------------------------------------------------------------------
  
}
