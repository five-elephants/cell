#include "socket_item.h"

namespace ast {

  Socket_item::Socket_item(Socket_direction const& direction,
      Node_if& name,
      Node_if& type)
    : Tree_base(),
      m_direction(direction),
      m_name(name),
      m_type(type) {
    register_branches({&m_name, &m_type});
  }

}
