#pragma once

#include <string>

#include "namespace.h"
#include "ast/node_if.h"

namespace ir {

  extern Namespace analyze(ast::Node_if const& ast_root,
      std::string const& defaultname = "default");

}
