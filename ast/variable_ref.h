#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Variable_ref : public Tree_base {
    public:
      Variable_ref(Node_if& identifier);

      virtual void visit();

      Node_if const& identifier() const { return m_identifier; }

    private:
      Node_if& m_identifier;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
