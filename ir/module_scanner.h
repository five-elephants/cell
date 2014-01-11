#pragma once

#include "ir/namespace_scanner.h"

namespace ir {

  class Module_scanner : public Namespace_scanner {
    public:
      Module_scanner(Module& mod)
        : Namespace_scanner(mod),
          m_mod(mod) {
      }

      virtual bool enter(ast::Node_if const& node); 


    private:
      Module& m_mod;

      std::shared_ptr<Object> insert_object(ast::Variable_def const& node);
      std::shared_ptr<Instantiation> insert_instantiation(ast::Module_instantiation const& node);

  };

}
