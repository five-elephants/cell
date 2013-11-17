#pragma once

#include "location.hh"
#include <vector>
#include <functional>

namespace gen {
  class Generator_if;
}

namespace ast {
  class Node_if {
    public:
      virtual void visit() = 0;
      virtual void visit(std::function<void(Node_if const&)> callback) const = 0;
      virtual void set_generator(gen::Generator_if& gen) = 0;
      virtual gen::Generator_if& get_generator() = 0;
      virtual void location(yy::location loc) = 0;
      virtual yy::location location() const = 0;
  };

}


