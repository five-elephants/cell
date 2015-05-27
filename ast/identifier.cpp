#include "identifier.h"

namespace ast {

  Identifier::Identifier(std::string const& txt)
    : Node_base(),
      m_identifier(txt) {
  }

}
