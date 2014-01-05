#pragma once

#include "ast/ast.h"
#include "ast/visitor.h"
#include "ir/namespace.h"

#include <iostream>

namespace ir {

  class Namespace_scanner : public ast::Visitor_base {
    public:
      Namespace_scanner(Namespace& ns) 
        : m_ns(ns),
          m_root(true) {
      }


      virtual ~Namespace_scanner() { }


      virtual bool enter(ast::Node_if const& node) {
        using namespace std;

        if( m_root ) {
          m_root = false;
          cout << "[root] Entering " << typeid(node).name() << '\n';
          return true;
        }

        cout << "Entering " << typeid(node).name() << '\n';
        if( typeid(node) == typeid(ast::Namespace_def) ) {
          m_ns.insert_namespace(dynamic_cast<ast::Namespace_def const&>(node));
          return false;
        } else if( typeid(node) == typeid(ast::Module_def) ) {
          m_ns.insert_module(dynamic_cast<ast::Module_def const&>(node));
          return false;
        }

        return true;
      }


      virtual bool visit(ast::Node_if const& node) {
        std::cout << "Visit " << typeid(node).name() << '\n';
        m_root = false;
        return true;
      }


    private:
      bool m_root;
      Namespace& m_ns;
  };

}
