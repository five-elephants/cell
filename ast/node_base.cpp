#include "ast/node_base.h"

#include "ast/visitor.h"
#include <iostream>
#include <sstream>

namespace ast {

  static std::stringstream nullstrm;



	Node_base::Node_base() {
	}

  void
  Node_base::visit(std::function<void(Node_if const&)> callback) const {
    callback(*this);
  }

  bool
  Node_base::accept(Visitor_if& visitor) const {
    return visitor.visit(*this);
  }

  void
  Node_base::location(yy::location loc) {
    m_location = loc;
  }

  yy::location
  Node_base::location() const {
    return m_location;
  }


}
