#pragma once

#include "ast/node_base.h"


namespace ast {

  class Phys_literal : public Tree_base {
    public:
      Phys_literal(int value, Node_if& unit)
        : Tree_base(),
          m_value(value),
          m_unit(unit) {
        register_branches({&m_unit});
      }

      int value() const { return m_value; }
      Node_if const& unit() const { return m_unit; }

    private:
      int m_value;
      Node_if& m_unit;
  };

}

/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
