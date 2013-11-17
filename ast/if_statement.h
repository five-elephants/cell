#pragma once

#include "ast/node_base.h"
#include <vector>
#include <stdexcept>

namespace ast {

	class If_statement : public Node_base {
		public:
			If_statement(Node_if& condition, Node_if& body);
			virtual ~If_statement();

			virtual void visit();
      virtual void visit(std::function<void(Node_if const&)> cb) const;
			virtual void set_generator(gen::Generator_if& g);

			Node_if& condition() { return m_condition; }
			Node_if& body() { return m_body; }
      bool has_else_body() const { return m_else_body != nullptr; }
      void else_body(Node_if& body) { m_else_body = &body; }
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
