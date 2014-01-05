#include "socket_def.h"

namespace ast {

  Socket_def::Socket_def(Node_if& identifier,
      std::vector<Node_if*> const& body)
    : m_identifier(identifier),
      m_body(body) {
    register_branches({&m_identifier});
    register_branch_lists({&m_body});
  }


  Socket_def::~Socket_def() {
  }

}

