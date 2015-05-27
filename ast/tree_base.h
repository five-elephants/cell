#pragma once

#include "ast/node_base.h"
#include "ast/visitor.h"

#include <initializer_list>
#include <vector>

namespace ast {

  class Tree_base : public Node_base {
    public:
      Tree_base()
        : Node_base() {
      }

      virtual void visit(std::function<void(Node_if const&)> callback) const;
      virtual bool accept(Visitor_if& visitor) const;

    protected:
      std::vector<Node_if*> m_nodes;
      std::vector< std::vector<Node_if*>* > m_node_lists;

      void register_branches(std::initializer_list<Node_if*> nodes);
      void register_branch_lists(std::initializer_list<std::vector<Node_if*>*> nodelist);
  };

}
