#include "module_scanner.h"

#include <set>
#include <sstream>
#include <stdexcept>

#include "find.hpp"
#include "scan_ast.h"
#include "types.h"
#include "streamop.h"
#include "make_array_type.h"

namespace ir {

  Module_scanner::Module_scanner(Module& mod) 
    : Namespace_scanner(mod),
      m_mod(mod) {
    on_enter_if_type<ast::Variable_def>(&Module_scanner::insert_object);
    on_enter_if_type<ast::Module_instantiation>(&Module_scanner::insert_instantiation);
    on_enter_if_type<ast::Process>(&Module_scanner::insert_process);
    on_enter_if_type<ast::Periodic>(&Module_scanner::insert_periodic);
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::enter(ast::Node_if const& node) {
    if( m_root ) {
      m_root = false;

      auto& mod = dynamic_cast<ast::Module_def const&>(node);
      if( mod.has_socket() ) {
        if( typeid(mod.socket()) == typeid(ast::Identifier) ) {
          auto& socket_name = dynamic_cast<ast::Identifier const&>(mod.socket()).identifier();
          m_mod.socket = find_socket(m_mod, socket_name);
          if( !m_mod.socket ) {
            std::stringstream strm;
            strm << node.location()
              << ": failed to find socket of name "
              << socket_name
              << " in module "
              << m_mod.name;
            throw std::runtime_error(strm.str());
          }
        } else if( typeid(mod.socket()) == typeid(ast::Socket_def) ) {
          auto& sock = dynamic_cast<ast::Socket_def const&>(mod.socket());
          return insert_socket(sock);
        }
      } else {
        m_mod.socket = Builtins::null_socket;
      }

      // generate code for module initialization
      // TODO
      //std::cout << "begin generation of module constructor code..." << std::endl;
      //m_mod.constructor_code = make_codeblock();
      //m_mod.constructor_code->scan_ast_module(node);
      //std::cout << "finished generation of module constructor code." << std::endl;

      return true;
    }

    return Namespace_scanner::enter(node);
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::insert_function(ast::Function_def const& node) {
    auto func = create_function(node);
    func->within_module = true;

    // TODO generate code for function body

    m_ns.functions[func->name] = func;

    return false;
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::insert_socket(ast::Socket_def const& sock) {
    auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
    auto s = std::shared_ptr<Socket>(new Socket(label));
    if( m_ns.sockets.count(s->name) > 0 )
      throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));
    if( m_ns.types.count(s->name) > 0 )
      throw std::runtime_error(std::string("Type with name ") + s->name +std::string(" already exists"));

    s->enclosing_ns = &m_ns;
    scan_ast(*s, sock);
    m_mod.socket = s;

    return false;
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::insert_instantiation(ast::Module_instantiation const& node) {
    std::shared_ptr<Instantiation> inst(new Instantiation);
    inst->name = dynamic_cast<ast::Identifier const&>(node.instance_name()).identifier();
    if( m_mod.instantiations.count(inst->name) > 0 )
      throw std::runtime_error(std::string("Instantiation with name ")
          + inst->name 
          + std::string(" already exists"));

    auto& module_name = dynamic_cast<ast::Identifier const&>(node.module_name()).identifier();
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
        auto& con_item = dynamic_cast<ast::Connection_item const&>(*i);
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
        auto& obj_name = dynamic_cast<ast::Identifier const&>(*i).identifier();
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

    return false;
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::insert_object(ast::Variable_def const& node) {
    std::shared_ptr<Object> obj(new Object);

    // get name
    obj->name = dynamic_cast<ast::Identifier const*>(&(node.identifier()))->identifier();
    if( m_mod.objects.count(obj->name) > 0 )
      throw std::runtime_error(std::string("Variable with name ")
          + obj->name
          + std::string(" already exists"));

    // get type
    if( typeid(node.type()) == typeid(ast::Identifier) ) {
      auto& type_name = dynamic_cast<ast::Identifier const&>(node.type()).identifier();
      obj->type = find_type(m_mod, type_name);
      if( !obj->type ) {
        std::stringstream strm;
        strm << node.type().location();
        strm << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }
    } else if( typeid(node.type()) == typeid(ast::Array_type) ) {
      auto& ar_type = dynamic_cast<ast::Array_type const&>(node.type());

      obj->type = make_array_type(m_mod, ar_type);
      m_mod.types[obj->type->name] = obj->type;
    }

    m_mod.objects[obj->name] = obj;
    return false;
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::insert_process(ast::Process const& node) {
    auto rv = std::make_shared<Process>();

    // generate code for process body
    auto cb = make_codeblock();
    cb->process(rv);
    cb->scan_ast(node.body());
    rv->code = cb;

    m_mod.processes.push_back(rv);

    return false;
  }
  //--------------------------------------------------------------------------------
  bool
  Module_scanner::insert_periodic(ast::Periodic const& node) {
    auto rv = std::make_shared<Periodic>();

    rv->period = Time(2, Time::ns);

    // generate code for process body
    auto cb = make_codeblock();
    cb->process(rv);
    cb->scan_ast(node.body());
    rv->code = cb;

    m_mod.periodicals.push_back(rv);

    return false;
  }
  //--------------------------------------------------------------------------------

}

/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
