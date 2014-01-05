#pragma once

#include "ast/ast.h"
#include "ast/visitor.h"
#include "ir/namespace.h"

namespace ir {

  class Namespace_scanner : public ast::Visitor_base {
    public:
      Namespace_scanner(Namespace& ns) 
        : m_ns(ns),
          m_root(true) {
      }


      virtual ~Namespace_scanner() { }


      virtual bool enter(ast::Node_if const& node) {
        if( m_root ) {
          m_root = false;
          return true;
        }

        if( typeid(node) == typeid(ast::Namespace_def) ) {
          m_ns.insert_namespace(dynamic_cast<ast::Namespace_def const&>(node));
          return false;
        } else if( typeid(node) == typeid(ast::Module_def) ) {
          m_ns.insert_module(dynamic_cast<ast::Module_def const&>(node));
          return false;
        }

        return true;
      }


    private:
      bool m_root;
      Namespace& m_ns;
  };

}
