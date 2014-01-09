#include "identifier.h"
#include "gen/generator_if.h"

namespace ast {

  Identifier::Identifier(std::string const& txt)
    : Node_base(),
      m_identifier(txt) {
  }


  void
  Identifier::visit() {
    get_generator().identifier(*this);
  }

}
