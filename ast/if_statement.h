#pragma once

#include "ast/tree_base.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace ast {

	class If_statement : public Tree_base {
		public:
			If_statement(Node_if& condition, Node_if& body);

			virtual void visit();

			Node_if& condition() { return m_condition; }
			Node_if& body() { return m_body; }
      bool has_else_body() const { return m_else_body != nullptr; }
      void else_body(Node_if& body) { 
        if( m_else_body != nullptr )
          std::remove(begin(m_nodes), end(m_nodes), m_else_body);
        m_else_body = &body;
        m_nodes.push_back(m_else_body);
      }
      Node_if& else_body() { 
        if( !has_else_body() )
          throw std::runtime_error("If_statement does not have else body");
        return *m_else_body;
      }

		private:
			Node_if& m_condition;
			Node_if& m_body;
      Node_if* m_else_body = nullptr;
	};

}
