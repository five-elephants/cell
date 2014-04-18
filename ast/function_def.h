#pragma once

#include "ast/tree_base.h"
#include "gen/generator_if.h"
#include <vector>

namespace ast {

  class Function_def : public Tree_base {
    public:
      Function_def(Node_if& identifier);
      Function_def(Node_if& identifier, Node_if& return_type);

      virtual void visit();

      void append_parameter(Node_if& node);
      void append_parameter(std::vector<Node_if*> const& nodes);
      void append_body(Node_if& node);

      Node_if& identifier() { return m_identifier; }
      Node_if const& identifier() const { return m_identifier; }
      Node_if& return_type() { return m_return_type; }
      Node_if const& return_type() const { return m_return_type; }
      void return_type(Node_if& t) { m_return_type = t; }
      Node_if const& body() const { return *m_body; }
      std::vector<Node_if*> const& parameters() const { return m_parameters; };

    private:
      Node_if& m_identifier;
      Node_if& m_return_type;
      std::vector<Node_if*> m_parameters;
      Node_if* m_body = nullptr;
  };

}


/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
