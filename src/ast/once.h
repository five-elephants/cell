#pragma once

#include "ast/tree_base.h"

namespace ast {

  class Once : public Tree_base {
    public:
      Once(Node_if& period, Node_if& body)
        : Tree_base(),
          m_time(period),
          m_body(body) {
        register_branches({&m_time, &m_body});
      }

    Node_if const& body() const { return m_body; }
    Node_if const& time() const { return m_time; }

    private:
      Node_if& m_time;
      Node_if& m_body;
  };

}

