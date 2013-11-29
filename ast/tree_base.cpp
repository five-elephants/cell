#include "ast/tree_base.h"

#include <iostream>

namespace ast {

  void
  Tree_base::visit(std::function<void(Node_if const&)> callback) const {
    Node_base::visit(callback);
    for(auto i : m_nodes) {
      std::cout << "visiting " << i->location() << std::endl;
      i->visit(callback);
    }

    for(auto lst : m_node_lists) {
      for(auto i : *lst) {
        i->visit(callback);
      }
    }
  }

  void
  Tree_base::set_generator(gen::Generator_if& gen) {
    Node_base::set_generator(gen);

    for(auto i : m_nodes) {
      i->set_generator(gen);
    }

    for(auto lst : m_node_lists) {
      for(auto i : *lst) {
        i->set_generator(gen);
      }
    }
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
