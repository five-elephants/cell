#include "connection_item.h"

namespace ast {

  Connection_item::Connection_item(Node_if& port_name, Node_if& signal_name)
    : Tree_base(),
      m_port_name(port_name),
      m_signal_name(signal_name) {
    register_branches({&m_port_name, &m_signal_name});
  }

}
