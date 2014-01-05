#pragma once

#include "ast/ast.h"
#include "ast/visitor.h"
#include "ir/namespace.h"

namespace ir {

  class Namespace_scanner : public ast::Visitor_base {
    public:
      Namespace_scanner(Namespace& ns) 
        : m_ns(ns) {
      }


      virtual ~Namespace_scanner() { }


      virtual bool enter(ast::Node_if const& node) {
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
      Namespace& m_ns;
  };

}
