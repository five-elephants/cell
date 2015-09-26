#pragma once

#include "namespace.h"
#include "ast/node_if.h"
#include "ast/ast_find.h"

#include <sstream>

namespace ir {

  template<typename Impl = No_impl>
  void scan_ast(Type<Impl>& socket, ast::Node_if const& tree) {
    auto port_nodes = ast::find_by_type<ast::Socket_item>(tree);

    for(auto i : port_nodes) {
      std::shared_ptr<Port<Impl>> port(new Port<Impl>);

      port->name = dynamic_cast<ast::Identifier const&>(i->name()).identifier();
      if( socket.elements.count(port->name) > 0 ) {
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

      socket.elements[port->name] = port;
    }
  }


}
