#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Namespace_def : public Tree_base {
    public:
      Namespace_def(Node_if& identifier);

      void append(Node_if& node);
			void append(std::vector<Node_if*> const& nodes);

			Node_if& identifier() { return m_identifier; }
			Node_if const& identifier() const { return m_identifier; }

      bool empty() const { return m_elements.empty(); }

		private:
			Node_if& m_identifier;
			std::vector<Node_if*> m_elements;
  };

}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
