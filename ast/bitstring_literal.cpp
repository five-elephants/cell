#include "ast/bitstring_literal.h"
#include "gen/generator_if.h"

namespace ast {

  Bitstring_literal::Bitstring_literal(std::string const& str)
    : Node_base(),
      m_bitstring(str) {
  }

  Bitstring_literal::~Bitstring_literal() {
  }

  void
  Bitstring_literal::visit() {
    get_generator().bitstring_literal(*this);
  }

}
