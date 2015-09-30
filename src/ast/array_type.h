#pragma once

#include "ast/tree_base.h"
#include "ast/constant_ref.h"

namespace ast {

  class Array_type : public Tree_base {
    public:
      Array_type(Node_if& base_type, Node_if& size_expr);

      Node_if const& base_type() const { return m_base_type; }
      Node_if const& size_expr() const { return m_size_expr; }


    private:
      Node_if& m_base_type;
      Node_if& m_size_expr;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
