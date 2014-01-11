#include "namespace_scanner.h"

#include "find.hpp"

#include <sstream>

namespace ir {

  //--------------------------------------------------------------------------------
  bool
  Namespace_scanner::enter(ast::Node_if const& node) {
    //using namespace std;

    if( m_root ) {
      m_root = false;
      //cout << "[root] Entering " << typeid(node).name() << '\n';
      return true;
    }

    //cout << "Entering " << typeid(node).name() << '\n';
    if( typeid(node) == typeid(ast::Namespace_def) ) {
      insert_namespace(dynamic_cast<ast::Namespace_def const&>(node));
      return false;
    } else if( typeid(node) == typeid(ast::Module_def) ) {
      insert_module(dynamic_cast<ast::Module_def const&>(node));
      return false;
    } else if( typeid(node) == typeid(ast::Socket_def) ) {
      insert_socket(dynamic_cast<ast::Socket_def const&>(node));
      return false;
    } else if( typeid(node) == typeid(ast::Function_def) ) {
      insert_function(dynamic_cast<ast::Function_def const&>(node));
      return false;
    }

    return true;
  }
  //--------------------------------------------------------------------------------
  bool
  Namespace_scanner::visit(ast::Node_if const& node) {
    //std::cout << "Visit " << typeid(node).name() << '\n';
    m_root = false;
    return true;
  }
  //--------------------------------------------------------------------------------
  std::shared_ptr<Module>
  Namespace_scanner::insert_module(ast::Module_def const& mod) {
    auto label = dynamic_cast<ast::Identifier const&>(mod.identifier()).identifier();
    auto m = std::shared_ptr<Module>(new Module(label));
    if( m_ns.modules.count(m->name) > 0 )
      throw std::runtime_error(std::string("Module with name ")+ m->name +std::string(" already exists"));

    m->enclosing_ns = &m_ns;
    m->scan_ast(mod);
    m_ns.modules[m->name] = m;

    return m;
  }
  //--------------------------------------------------------------------------------
  std::shared_ptr<Namespace>
  Namespace_scanner::insert_namespace(ast::Namespace_def const& ns) {
    auto label = dynamic_cast<ast::Identifier const*>(&(ns.identifier()))->identifier();
    auto n = std::shared_ptr<Namespace>(new Namespace(label));
    if( m_ns.namespaces.count(n->name) > 0 )
      throw std::runtime_error(std::string("Namespace with name ")+ n->name +std::string(" already exists"));

    n->enclosing_ns = &m_ns;
    n->scan_ast(ns);
    m_ns.namespaces[n->name] = n;

    return n;
  }
  //--------------------------------------------------------------------------------
  std::shared_ptr<Socket>
  Namespace_scanner::insert_socket(ast::Socket_def const& sock) {
    auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
    auto s = std::shared_ptr<Socket>(new Socket(label));
    if( m_ns.sockets.count(s->name) > 0 )
      throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));
    if( m_ns.types.count(s->name) > 0 )
      throw std::runtime_error(std::string("Type with name ") + s->name +std::string(" already exists"));

    s->enclosing_ns = &m_ns;
    s->scan_ast(sock);
    m_ns.sockets[s->name] = s;
    m_ns.types[s->name] = s;

    return s;
  }
  //--------------------------------------------------------------------------------
  std::shared_ptr<Function>
  Namespace_scanner::insert_function(ast::Function_def const& node) {
    std::shared_ptr<Function> func(new Function);
    func->name = dynamic_cast<ast::Identifier const*>(&(node.identifier()))->identifier();
    if( m_ns.functions.count(func->name) > 0 )
      throw std::runtime_error(std::string("Function with name ")+ func->name +std::string(" already exists"));

    auto type_name = dynamic_cast<ast::Identifier const*>(&(node.return_type()))->identifier();
    func->return_type = find_type(m_ns, type_name);
    if( !func->return_type ) {
      std::stringstream strm;
      strm << node.return_type().location();
      strm << ": return type '" << type_name << "' not found.";
      throw std::runtime_error(strm.str());
    }
    m_ns.functions[func->name] = func;

    return func;
  } 
  //--------------------------------------------------------------------------------

}
