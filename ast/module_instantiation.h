#pragma once

#include "tree_base.h"
#include "identifier.h"

namespace ast {

  class Module_instantiation : public Tree_base {
    public:
      Module_instantiation(Node_if& module_name,
          Node_if& instance_name, 
          std::vector<Node_if*>& connection_items);
      
      virtual void visit() {}

      Identifier const& module_name() const {
        return dynamic_cast<Identifier const&>(m_module_name);
      }

      Identifier const& instance_name() const {
        return dynamic_cast<Identifier const&>(m_instance_name);
      }

      std::vector<Node_if*> const& connection_items() const {
        return m_connection_items;
      }

    private:
      Node_if& m_module_name;
      Node_if& m_instance_name;
      std::vector<Node_if*>& m_connection_items;
  };

}

