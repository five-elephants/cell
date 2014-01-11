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
      port->type = find_type(*enclosing_ns, type_name);
      if( !port->type ) {
        std::stringstream strm;
        strm << i->type().location();
        strm << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }

      ports[port->name] = port;
    }
  }
  //--------------------------------------------------------------------------------
  void
  Module::scan_ast(ast::Node_if const& tree) {
    Module_scanner scanner(*this);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------
  void
  Namespace::scan_ast(ast::Node_if const& tree) {
    Namespace_scanner scanner(*this);
    tree.accept(scanner);
  }
  //--------------------------------------------------------------------------------
  
}
