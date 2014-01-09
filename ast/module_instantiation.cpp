#include "module_instantiation.h"

namespace ast {

  Module_instantiation::Module_instantiation(Node_if& module_name,
          Node_if& instance_name, 
          std::vector<Node_if*>& connection_items)
    : Tree_base(),
      m_module_name(module_name),
      m_instance_name(instance_name),
      m_connection_items(connection_items) {
    register_branch_lists({&m_connection_items});
    register_branches({&m_module_name, &m_instance_name});
  }

}
