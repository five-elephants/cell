#pragma once

#include <vector>
#include "tree_base.h"

namespace ast {

  class Socket_def : public Tree_base {
    public:
      Socket_def(Node_if& identifier,
          std::vector<Node_if*> const& body);

      virtual void visit() {}

			Node_if& identifier() { return m_identifier; }
			Node_if const& identifier() const { return m_identifier; }

    private:
      Node_if& m_identifier;
      std::vector<Node_if*> m_body;
  };

}
