#include "scan_ast.h"

#include "ast/ast_find.h"
#include "namespace_scanner.h"
#include "module_scanner.h"
#include "find.hpp"

#include <stdexcept>
#include <sstream>
#include <set>

namespace ir {

  //--------------------------------------------------------------------------------
  void scan_ast(Socket& socket, ast::Node_if const& tree) {
    auto port_nodes = ast::find_by_type<ast::Socket_item>(tree);
    
    for(auto i : port_nodes) {
      std::shared_ptr<Port> port(new Port);

      port->name = dynamic_cast<ast::Identifier const&>(i->name()).identifier();
      if( socket.ports.count(port->name) > 0 ) {
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
      port->type = find_type(*(socket.enclosing_ns), type_name);
      if( !port->type ) {
        std::stringstream strm;
        strm << i->type().location();
        strm << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }

      socket.ports[port->name] = port;
    }
  }
  //--------------------------------------------------------------------------------
  void scan_ast(Module& mod, ast::Node_if const& tree) {
    Module_scanner scanner(mod);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------
  void scan_ast(Namespace& ns, ast::Node_if const& tree) {
    Namespace_scanner scanner(ns);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------
  
}
