#pragma once

#include "tree_base.h"

#include "socket_direction.h"

namespace ast {

  class Socket_item : public Tree_base {
    public:
      Socket_item(Socket_direction const& direction,
          Node_if& name,
          Node_if& type);

      Socket_direction direction() const { return m_direction; }
      Node_if const& name() const { return m_name; }
      Node_if const& type() const { return m_type; }

    private:
      Socket_direction m_direction;
      Node_if& m_name;
      Node_if& m_type;
  };

}
