#pragma once

#include "tree_base.h"
#include "identifier.h"

namespace ast {

  class Connection_item : public Tree_base {
    public:
      Connection_item(Node_if& port_name, Node_if& expression);

      virtual void visit() {};

      Identifier const& port_name() const {
        return dynamic_cast<Identifier const&>(m_port_name);
      }

      Node_if const& expression() const {
        return m_expression;
      }

    private:
      Node_if& m_port_name;
      Node_if& m_expression;
  };

}

/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
