#include "connection_item.h"

namespace ast {

  Connection_item::Connection_item(Node_if& port_name, Node_if& expression)
    : Tree_base(),
      m_port_name(port_name),
      m_expression(expression) {
    register_branches({&m_port_name, &m_expression});
  }

}
