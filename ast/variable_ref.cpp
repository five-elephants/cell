#include "ast/variable_ref.h"

namespace ast {

  Variable_ref::Variable_ref(Node_if& id)
      : Tree_base(),
        m_identifier(id) {
    register_branches({&m_identifier});
  }


  void
  Variable_ref::visit() {
  }

}
