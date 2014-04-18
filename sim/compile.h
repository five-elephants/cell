#pragma once

#include <string>

#include "ir/namespace.h"
#include "ast/node_if.h"

namespace sim {

  extern ir::Namespace compile(ast::Node_if const& ast_root,
      std::string const& defaultname = "default");

}

