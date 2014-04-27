#pragma once

#include <string>
#include <memory>
#include <tuple>
#include <llvm/IR/Module.h>

#include "ir/namespace.h"
#include "ast/node_if.h"

namespace sim {

  extern std::tuple<ir::Namespace, std::shared_ptr<llvm::Module>> compile(ast::Node_if const& ast_root,
      std::string const& defaultname = "default");

}

