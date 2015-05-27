#pragma once

#include "ast/node_base.h"
#include <string>

namespace ast {

  class Bitstring_literal : public Node_base {
    public:
      Bitstring_literal(std::string const& str);

      std::string& bitstring() { return m_bitstring; }

    private:
      std::string m_bitstring;
  };

}
