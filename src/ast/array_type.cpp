#include "ast/array_type.h"


namespace ast {

  Array_type::Array_type(Node_if& base_type, Node_if& size_expr)
    : Tree_base(),
      m_base_type(base_type),
      m_size_expr(size_expr) {
    register_branches({&m_base_type, &m_size_expr});
  }

}

