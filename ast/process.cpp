#include "ast/process.h"

namespace ast {

  Process::Process(Node_if& body)
    : Tree_base(),
      m_body(body) {
    register_branches({&m_body});
  }

}
