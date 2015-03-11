#include "ast/variable_ref.h"

namespace ast {

  Variable_ref::Variable_ref(Node_if& expression)
      : Tree_base(),
        m_expression(expression) {
    register_branches({&m_expression});
  }


  void
  Variable_ref::visit() {
  }

}
