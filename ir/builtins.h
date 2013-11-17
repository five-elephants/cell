#pragma once

#include "namespace.h"

namespace ir {

  struct Builtins {
    static std::map<Label, std::shared_ptr<Type>> types;
    static std::map<Label, std::shared_ptr<Object>> objects;
    static std::map<Label, std::shared_ptr<Function>> functions;
  };

}
