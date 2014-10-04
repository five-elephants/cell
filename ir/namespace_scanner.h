#pragma once

#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "ir/namespace.h"
#include "find.hpp"
#include "scan_ast.h"
#include "types.h"
#include "streamop.h"
#include "make_array_type.h"

#include <sstream>
#include <set>
#include <stdexcept>


//#include <iostream>

namespace ir {

  //template<typename Impl = No_impl> class Module_scanner;


  template<typename Impl = No_impl>
  class Namespace_scanner : public ast::Scanner_base<Namespace_scanner<Impl>> {
    using Scanner_base = ast::Scanner_base<Namespace_scanner<Impl>>;

    public:
      Namespace_scanner(Namespace<Impl>& ns)
        : m_ns(ns),
          m_root(true) {

        this->template on_enter_if_type<ast::Namespace_def>(&Namespace_scanner::insert_namespace);
        this->template on_enter_if_type<ast::Module_def>(&Namespace_scanner::insert_module);
        this->template on_enter_if_type<ast::Socket_def>(&Namespace_scanner::insert_socket);
        this->template on_enter_if_type<ast::Function_def>(&Namespace_scanner::insert_function);
      }


      virtual bool enter(ast::Node_if const& node) {
        if( m_root ) {
          m_root = false;
          return true;
        }

        return Scanner_base::enter(node);
      }


      virtual bool visit(ast::Node_if const& node) {
        m_root = false;
        return Scanner_base::visit(node);
      }


    protected:
      bool m_root;
      Namespace<Impl>& m_ns;

      virtual bool insert_module(ast::Module_def const& mod); 


      virtual bool insert_namespace(ast::Namespace_def const& ns) {
        auto label = dynamic_cast<ast::Identifier const*>(&(ns.identifier()))->identifier();
        auto n = std::make_shared<Namespace<Impl>>(label);
        //auto n = std::shared_ptr<Namespace<Impl>>(new Namespace(label));
        if( m_ns.namespaces.count(n->name) > 0 )
          throw std::runtime_error(std::string("Namespace with name ")+ n->name +std::string(" already exists"));

        n->enclosing_ns = &m_ns;

        Namespace_scanner<Impl> scanner(*n);
        ns.accept(scanner);

        m_ns.namespaces[n->name] = n;

        return false;
      }


      virtual bool insert_socket(ast::Socket_def const& sock) {
        auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
        auto s = std::make_shared<Socket<Impl>>(label);
        //auto s = std::shared_ptr<Socket>(new Socket(label));
        if( m_ns.sockets.count(s->name) > 0 )
          throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));
        if( m_ns.types.count(s->name) > 0 )
          throw std::runtime_error(std::string("Type with name ") + s->name +std::string(" already exists"));

        s->enclosing_ns = &m_ns;
        scan_ast(*s, sock);
        m_ns.sockets[s->name] = s;
        m_ns.types[s->name] = s;

        return false;
      }


      virtual bool insert_function(ast::Function_def const& node) {
        auto func = create_function(node);
        m_ns.functions[func->name] = func;

        return false;
      }


      virtual std::shared_ptr<ir::Function<Impl>> create_function(ast::Function_def const& node) {
        std::shared_ptr<Function<Impl>> func(new Function<Impl>());

        // get name
        func->name = dynamic_cast<ast::Identifier const*>(&(node.identifier()))->identifier();
        if( m_ns.functions.count(func->name) > 0 )
          throw std::runtime_error(std::string("Function with name ")+ func->name +std::string(" already exists"));

        // get return type
        auto type_name = dynamic_cast<ast::Identifier const*>(&(node.return_type()))->identifier();
        func->return_type = find_type(m_ns, type_name);
        if( !func->return_type ) {
          std::stringstream strm;
          strm << node.return_type().location();
          strm << ": return type '" << type_name << "' not found.";
          throw std::runtime_error(strm.str());
        }

        // get parameters
        auto params = node.parameters();
        for(auto p_node : params) {
          if( typeid(*p_node) != typeid(ast::Function_param) )
            throw std::runtime_error("function parameter is not of type Function_param ("
                + std::string(typeid(p_node).name())
                + std::string(" instead)"));

          auto p = dynamic_cast<ast::Function_param*>(p_node);
          auto p_ir = std::make_shared<Object<Impl>>();
          p_ir->name = dynamic_cast<ast::Identifier const&>(p->identifier()).identifier();
          //if( func->parameters.count(p_ir->name) > 0 )
          if( std::any_of(func->parameters.begin(),
                func->parameters.end(),
                [p_ir](std::shared_ptr<Object<Impl>> const& i) { return i->name == p_ir->name; }) ) {
            throw std::runtime_error(std::string("Parameter with name ")
                + p_ir->name
                + std::string(" already defined"));
          }

          auto type_name = dynamic_cast<ast::Identifier const&>(p->type()).identifier();
          p_ir->type = find_type(m_ns, type_name);
          if( !p_ir->type ) {
            std::stringstream strm;
            strm << p->type().location();
            strm << ": typename '" << type_name << "' not found.";
            throw std::runtime_error(strm.str());
          }

          //func->parameters[p_ir->name] = p_ir;
          func->parameters.push_back(p_ir);
        }

        m_ns.functions[func->name] = func;

        return func;
      }
  };










  template<typename Impl = No_impl>
  class Module_scanner : public Namespace_scanner<Impl> {
    using Namespace_scanner = Namespace_scanner<Impl>;

    public:
      Module_scanner(Module<Impl>& mod)
        : Namespace_scanner(mod),
          m_mod(mod) {
        this->template on_enter_if_type<ast::Variable_def>(&Module_scanner::insert_object);
        this->template on_enter_if_type<ast::Module_instantiation>(&Module_scanner::insert_instantiation);
        this->template on_enter_if_type<ast::Process>(&Module_scanner::insert_process);
        this->template on_enter_if_type<ast::Periodic>(&Module_scanner::insert_periodic);
      }


      virtual bool enter(ast::Node_if const& node) {
        if( this->m_root ) {
          this->m_root = false;

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
            m_mod.socket = Builtins<Impl>::null_socket;
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


    protected:
      Module<Impl>& m_mod;

      virtual bool insert_function(ast::Function_def const& node) {
        auto func = this->create_function(node);
        func->within_module = true;

        // TODO generate code for function body

        this->m_ns.functions[func->name] = func;

        return false;
      }


      virtual bool insert_socket(ast::Socket_def const& sock) {
        auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
        auto s = std::make_shared<Socket<Impl>>(label);
        //auto s = std::shared_ptr<Socket>(new Socket(label));
        if( this->m_ns.sockets.count(s->name) > 0 )
          throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));
        if( this->m_ns.types.count(s->name) > 0 )
          throw std::runtime_error(std::string("Type with name ") + s->name +std::string(" already exists"));

        s->enclosing_ns = &this->m_ns;
        scan_ast(*s, sock);
        m_mod.socket = s;

        return false;
      }


      //std::shared_ptr<Function> insert_function(ast::Function_def const& func);
      virtual bool insert_object(ast::Variable_def const& node) {
        std::shared_ptr<Object<Impl>> obj(new Object<Impl>());

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


      virtual bool insert_instantiation(ast::Module_instantiation const& node) {
        std::shared_ptr<Instantiation<Impl>> inst(new Instantiation<Impl>);
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

            std::shared_ptr<Port_assignment<Impl>> port_assign(new Port_assignment<Impl>());
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

                std::shared_ptr<Port_assignment<Impl>> port_assign(new Port_assignment<Impl>);
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


      virtual bool insert_process(ast::Process const& node) {
        auto rv = std::make_shared<Process<Impl>>();

        // generate code for process body
        //auto cb = make_codeblock();
        //cb->process(rv);
        //cb->scan_ast(node.body());
        //rv->code = cb;

        m_mod.processes.push_back(rv);

        return false;
      }


      virtual bool insert_periodic(ast::Periodic const& node) {
        auto rv = std::make_shared<Periodic<Impl>>();

        rv->period = Time(2, Time::ns);

        // generate code for process body
        //auto cb = make_codeblock();
        //cb->process(rv);
        //cb->scan_ast(node.body());
        //rv->code = cb;

        m_mod.periodicals.push_back(rv);

        return false;
      }
  };


  


  template<typename Impl>
  bool
  Namespace_scanner<Impl>::insert_module(ast::Module_def const& mod) {
    auto label = dynamic_cast<ast::Identifier const&>(mod.identifier()).identifier();
    auto m = std::make_shared<Module<Impl>>(label);
    if( m_ns.modules.count(m->name) > 0 )
      throw std::runtime_error(std::string("Module with name ")+ m->name +std::string(" already exists"));

    m->enclosing_ns = &m_ns;
    Module_scanner<Impl> scanner(*m);
    mod.accept(scanner);
    m_ns.modules[m->name] = m;

    return false;
  }

}
