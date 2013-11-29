#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Variable_def : public Tree_base {
    public:
      Variable_def(Node_if& identifier);
      Variable_def(Node_if& identifier,
          Node_if& type,
          Node_if& expression);
      virtual ~Variable_def();

      virtual void visit();

      Node_if& identifier() { return m_identifier; }
      Node_if const& identifier() const { return m_identifier; }
      Node_if& type() { return *m_type; }
      Node_if const& type() const { return *m_type; }
      void type(Node_if& node) { 
        std::remove(begin(m_nodes), end(m_nodes), m_type);
        if( m_is_type_owner ) {
          delete m_type;
        }
        m_type = &node;
        m_nodes.push_back(m_type);
      }
      Node_if& expression() { return *m_expression; }
      void expression(Node_if& node) { 
        std::remove(begin(m_nodes), end(m_nodes), m_expression);
        if( m_is_expression_owner ) {
          delete m_expression;
        }
        m_expression = &node;
        m_nodes.push_back(m_expression);
      }

      bool without_expression() const { return m_is_expression_owner; }

    private:
      Node_if& m_identifier;
      Node_if* m_type;
      bool m_is_type_owner = false;
      Node_if* m_expression;
      bool m_is_expression_owner = false;

  };

}

