#include "module_scanner.h"

#include <set>
#include <sstream>
#include <stdexcept>

#include "find.hpp"
#include "types.h"
#include "streamop.h"

namespace ir {

  void
  Module_scanner::insert_instantiation(ast::Module_instantiation const& node) {
    std::shared_ptr<Instantiation> inst(new Instantiation);
    inst->name = dynamic_cast<ast::Identifier const&>(node.instance_name()).identifier();
    if( m_mod.instantiations.count(inst->name) > 0 )
      throw std::runtime_error(std::string("Instantiation with name ")
          + inst->name 
          + std::string(" already exists"));

    auto module_name = dynamic_cast<ast::Identifier const&>(node.module_name()).identifier();
    inst->module = find_module(m_mod, module_name);
    if( !inst->module ) {
      std::stringstream strm;
      strm << node.module_name().location();
      strm << ": module '" << module_name << "' not found.";
      throw std::runtime_error(strm.str());
    }

    std::set<Label> matched_ports;
    for(auto& i : node.connection_items()) {
      if( typeid(*i) == typeid(ast::Connection_item) ) {
        auto con_item = dynamic_cast<ast::Connection_item const&>(*i);
        auto port_name = con_item.port_name().identifier();
        auto signal_name = con_item.signal_name().identifier();

        if( matched_ports.count(port_name) > 0 ) {
          std::stringstream strm;
          strm << con_item.port_name().location();
          strm << ": Port already connected";
          throw std::runtime_error(strm.str());
        }

        std::shared_ptr<Port_assignment> port_assign(new Port_assignment);
        port_assign->port = find_port(*(inst->module), port_name);
        if( !port_assign->port ) {
          std::stringstream strm;
          strm << con_item.port_name().location();
          strm << ": port '" << port_name << "' not found.";
          throw std::runtime_error(strm.str());
        }

        port_assign->object = find_object(m_mod, signal_name);
        if( !port_assign->object ) {
          std::stringstream strm;
          strm << con_item.signal_name().location();
          strm << ": assigned object '" << signal_name << "' not found.";
          throw std::runtime_error(strm.str());
        }

        if( !type_compatible(*(port_assign->port->type), *(port_assign->object->type)) ) {
          std::stringstream strm;
          strm << con_item.location();
          strm << ": incompatible types in port assignment: expected type '"
            << *(port_assign->port->type)
            << "' got '"
            << *(port_assign->object->type) << "'";
          throw std::runtime_error(strm.str());
        }

        inst->connection.push_back(port_assign);
        matched_ports.insert(port_name);
      } else if( typeid(*i) == typeid(ast::Identifier) ) {
        auto obj_name = dynamic_cast<ast::Identifier const&>(*i).identifier();
        auto assignee = find_object(m_mod, obj_name);

        if( !assignee ) {
          std::stringstream strm;
          strm << i->location()
            << ": object '" << obj_name << "' not found";
          throw std::runtime_error(strm.str());
        }

        auto assignee_socket = find_socket(m_mod, assignee->type->name);
        if( !assignee_socket ) {
          std::stringstream strm;
          strm << i->location()
            << ": object '" << obj_name << "' of type '" << assignee->type->name
            << "' is not a socket";
          throw std::runtime_error(strm.str());
        }

        for(auto assignee_port_pair : assignee_socket->ports) {
          auto port_name = assignee_port_pair.first;
          auto assignee_port = assignee_port_pair.second;
          auto searchit = m_mod.socket->ports.find(port_name);
          if( searchit != m_mod.socket->ports.end() ) {
            auto it = searchit->second;
            if( !type_compatible(*(it->type), *(assignee_port->type)) ) {
              std::stringstream strm;
              strm << i->location()
                << ": name match for port '" << port_name << "'"
                << " but no type match (expected: "
                << *(it->type)
                << ", got: "
                << *(assignee_port->type)
                << ")";
              throw std::runtime_error(strm.str());
            }

            if( matched_ports.count(assignee_port->name) > 0 ) {
              std::stringstream strm;
              strm << i->location()
                << ": port '" << assignee_port->name << "' already matched";
              throw std::runtime_error(strm.str());
            }

            std::shared_ptr<Port_assignment> port_assign(new Port_assignment);
            port_assign->port = it;
            // XXX assign object to element of composite type socket
            inst->connection.push_back(port_assign);
            matched_ports.insert(assignee_port->name);
          }
        }
          
      }
    }

    m_mod.instantiations[inst->name] = inst;
  }

}

/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
