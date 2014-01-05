#pragma once

#include <string>

namespace ir {

  extern Namespace analyze(Node_if const& ast_root,
      std::string const& defaultname = "default");

}
