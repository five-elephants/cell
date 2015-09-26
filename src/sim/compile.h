#pragma once

#include <string>
#include <memory>
#include <tuple>
#include <llvm/IR/Module.h>

#include "sim/llvm_namespace.h"
#include "ast/node_if.h"

namespace sim {

  extern sim::Llvm_library compile(ast::Node_if const& ast_root,
      std::string const& defaultname = "default");

}

