#include "ast/array_type.h"


namespace ast {

  Array_type::Array_type(Node_if& base_type, Node_if& size_constant)
    : Tree_base(),
      m_base_type(base_type),
      m_size_constant(size_constant) {
    register_branches({&m_base_type, &m_size_constant});
  }

}

