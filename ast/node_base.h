#pragma once

#include "ast/node_if.h"

namespace ast {

  class Node_base : public Node_if {
    public:
      Node_base();

      virtual void visit(std::function<void(Node_if const&)> callback) const;
      virtual bool accept(Visitor_if& visitor) const;

      virtual void location(yy::location loc);
      virtual yy::location location() const;

    private:
      yy::location m_location;
  };

}
