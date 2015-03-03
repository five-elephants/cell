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


namespace ir {

  template<typename Impl = No_impl>
  class Namespace_scanner : public ast::Scanner_base<Namespace_scanner<Impl>> {
    //using Scanner_base = ast::Scanner_base<Namespace_scanner<Impl>>;

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

        return ast::Scanner_base<Namespace_scanner<Impl>>::enter(node);
      }


      virtual bool visit(ast::Node_if const& node) {
        m_root = false;
        return ast::Scanner_base<Namespace_scanner<Impl>>::visit(node);
      }


    protected:
      bool m_root;
      Namespace<Impl>& m_ns;

      virtual bool insert_module(ast::Module_def const& mod);
      virtual bool insert_namespace(ast::Namespace_def const& ns);
      virtual bool insert_socket(ast::Socket_def const& sock);
      virtual bool insert_function(ast::Function_def const& node);

      virtual std::shared_ptr<ir::Namespace<Impl>> create_namespace(ast::Namespace_def const& node);
      virtual std::shared_ptr<ir::Module<Impl>> create_module(ast::Module_def const& node);
      virtual std::shared_ptr<ir::Function<Impl>> create_function(ast::Function_def const& node);
  };



  template<typename Impl = No_impl>
  class Module_scanner : public Namespace_scanner<Impl> {
    //using Base = Namespace_scanner<Impl>;

    public:
      Module_scanner(Module<Impl>& mod)
        : Namespace_scanner<Impl>(mod),
          m_mod(mod) {
        this->template on_enter_if_type<ast::Variable_def>(&Module_scanner::insert_object);
        this->template on_enter_if_type<ast::Module_instantiation>(&Module_scanner::insert_instantiation);
        this->template on_enter_if_type<ast::Process>(&Module_scanner::insert_process);
        this->template on_enter_if_type<ast::Periodic>(&Module_scanner::insert_periodic);
        this->template on_enter_if_type<ast::Once>(&Module_scanner::insert_once);
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

        return Namespace_scanner<Impl>::enter(node);
      }


    protected:
      Module<Impl>& m_mod;

      virtual bool insert_function(ast::Function_def const& node);
      virtual bool insert_socket(ast::Socket_def const& sock);
      virtual bool insert_object(ast::Variable_def const& node);
      virtual bool insert_instantiation(ast::Module_instantiation const& node);
      virtual bool insert_process(ast::Process const& node);
      virtual bool insert_periodic(ast::Periodic const& node);
      virtual bool insert_once(ast::Once const& node);

      virtual std::shared_ptr<ir::Function<Impl>> create_function(ast::Function_def const& node);
      virtual std::shared_ptr<ir::Object<Impl>> create_object(ast::Variable_def const& node);
      virtual std::shared_ptr<ir::Process<Impl>> create_process(ast::Process const& node);
  };

}


#include "namespace_scanner.cpp"
#include "module_scanner.cpp"

