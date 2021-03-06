#include "ast/tree_base.h"

namespace ast {

  void
  Tree_base::visit(std::function<void(Node_if const&)> callback) const {
    Node_base::visit(callback);
    for(auto i : m_nodes) {
      i->visit(callback);
    }

    for(auto lst : m_node_lists) {
      for(auto i : *lst) {
        i->visit(callback);
      }
    }
  }

  bool
  Tree_base::accept(Visitor_if& visitor) const {
    if( visitor.enter(*this) ) {
      bool terminate = false;

      for(auto i : m_nodes) {
        if( !i->accept(visitor) ) {
          terminate = true;
          break;
        }
      }

      if( !terminate ) {
        for(auto lst : m_node_lists) {
          for(auto i : *lst) {
            if( !i->accept(visitor) ) {
              terminate = true;
              break;
            }

            if( terminate )
              break;
          }
        }
      }
    }

    return visitor.leave(*this);
  }


  void
  Tree_base::register_branches(std::initializer_list<Node_if*> nodes) {
    m_nodes = std::vector<Node_if*>(nodes);
  }

  void
  Tree_base::register_branch_lists(std::initializer_list<std::vector<Node_if*>*> nodelist) {
    m_node_lists = std::vector< std::vector<Node_if*>* >(nodelist);
  }

}
