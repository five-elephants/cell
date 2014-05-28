#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Array_type : public Tree_base {
    public:
      Array_type(Node_if& base_type, int size);

      virtual void visit() {}

      Node_if const& base_type() const { return m_base_type; }
      int size() const { return m_size; }


    private:
      Node_if& m_base_type;
      int m_size;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
