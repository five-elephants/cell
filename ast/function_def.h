#pragma once

#include "ast/tree_base.h"
#include "gen/generator_if.h"
#include <vector>

namespace ast {

  class Function_def : public Tree_base<Function_def> {
    friend class Tree_base<Function_def>;

    public:
      Function_def(Node_if& identifier);
      Function_def(Node_if& identifier, Node_if& return_type);
      virtual ~Function_def() {}

      virtual void set_generator(gen::Generator_if& g);
      virtual void visit();
      //virtual void visit(std::function<void(Node_if const&)> cb) const;

      void append_parameter(Node_if& node);
      void append_parameter(std::vector<Node_if*> const& nodes);
      void append_body(Node_if& node);
      void append_body(std::vector<Node_if*> const& nodes);

      Node_if& identifier() { return m_identifier; }
      Node_if const& identifier() const { return m_identifier; }
      Node_if& return_type() { return m_return_type; }
      Node_if const& return_type() const { return m_return_type; }
      void return_type(Node_if& t) { m_return_type = t; }

    private:
      Node_if& m_identifier;
      Node_if& m_return_type;
      std::vector<Node_if*> m_parameters;
      std::vector<Node_if*> m_body;

      const std::tuple<decltype(m_identifier)&,
          decltype(m_return_type)&,
          decltype(m_parameters)&,
          decltype(m_body)&> m_branches { m_identifier, m_return_type, m_parameters, m_body };
  };

}


/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
