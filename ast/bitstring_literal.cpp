#include "ast/bitstring_literal.h"

namespace ast {

  Bitstring_literal::Bitstring_literal(std::string const& str)
    : Node_base(),
      m_bitstring(str) {
  }

}
