#pragma once

#include "ast/tree_base.h"
#include "ast/identifier.h"


namespace ast {

  class Constant_ref : public Tree_base {
    public:
      Constant_ref(Node_if& identifier)
        : Tree_base(),
          m_identifier(identifier) {
        register_branches({&m_identifier});
      }

      Identifier const& identifier() const {
        return dynamic_cast<Identifier const&>(m_identifier);
      }

    private:
      Node_if& m_identifier;
  };

}


/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
