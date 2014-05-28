#include "ast/array_type.h"


namespace ast {

  Array_type::Array_type(Node_if& base_type, int size)
    : Tree_base(),
      m_base_type(base_type),
      m_size(size) {
    register_branches({&m_base_type});
  }

}

