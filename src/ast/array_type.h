#pragma once

#include "ast/tree_base.h"
#include "ast/constant_ref.h"

namespace ast {

  class Array_type : public Tree_base {
    public:
      Array_type(Node_if& base_type, Node_if& size_constant);

      Node_if const& base_type() const { return m_base_type; }
      Constant_ref const& size_constant() const {
        return dynamic_cast<Constant_ref const&>(m_size_constant);
      }


    private:
      Node_if& m_base_type;
      Node_if& m_size_constant;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
