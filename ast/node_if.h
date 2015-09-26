#pragma once

#include "location.hh"
#include <vector>
#include <functional>

/** Abstract syntax tree namespace */
namespace ast {
  class Visitor_if;

  class Node_if {
    public:
      virtual ~Node_if() {};

      virtual void visit(std::function<void(Node_if const&)> callback) const = 0;
      virtual bool accept(Visitor_if& visitor) const = 0;
      virtual void location(yy::location loc) = 0;
      virtual yy::location location() const = 0;
  };

}


