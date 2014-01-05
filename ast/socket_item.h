#pragma once

#include "tree_base.h"

#include "socket_direction.h"

namespace ast {

  class Socket_item : public Tree_base {
    public:
      Socket_item(Socket_direction const& direction,
          Node_if& name,
          Node_if& type);
      virtual ~Socket_item();

      virtual void visit() { };

    private:
      Socket_direction m_direction;
      Node_if& m_name;
      Node_if& m_type;
  };

}
