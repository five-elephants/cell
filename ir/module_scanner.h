#pragma once

#include "ir/namespace_scanner.h"

namespace ir {

  class Module_scanner : public Namespace_scanner {
    public:
      Module_scanner(Module& mod)
        : Namespace_scanner(mod),
          m_mod(mod) {
      }


      virtual ~Module_scanner() {
      }


      virtual bool enter(ast::Node_if const& node) {
        if( m_root ) {
          m_root = false;
          return true;
        }

        if( !Namespace_scanner::enter(node) ) {
          return false;
        } else if( typeid(node) == typeid(ast::Variable_def) ) {
          m_mod.insert_object(dynamic_cast<ast::Variable_def const&>(node));
          return false;
        } else if( typeid(node) == typeid(ast::Module_instantiation) ) {
          m_mod.insert_instantiation(dynamic_cast<ast::Module_instantiation const&>(node));
          return false;
        }
        
        return true;
      }


    private:
      Module& m_mod;
  };

}
