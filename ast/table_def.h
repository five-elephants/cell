#pragma once

#include "ast/tree_base.h"
#include <vector>
#include <algorithm>

namespace ast {

  class Table_def : public Tree_base {
    public:
      Table_def(Node_if* identifier,
          std::vector<Node_if*>* value_type_qname,
          std::vector<Node_if*>* items)
        : Tree_base(),
          m_identifier(identifier) {
        m_value_type.resize(value_type_qname->size());
        std::copy(value_type_qname->begin(),
            value_type_qname->end(),
            m_value_type.begin());

        m_items.resize(items->size());
        std::copy(items->begin(), items->end(), m_items.begin());

        register_branches({m_identifier});
        register_branch_lists({&m_value_type, &m_items});
      }

      virtual void visit() {}


    private:
      Node_if* m_identifier;
      std::vector<Node_if*> m_value_type;
      std::vector<Node_if*> m_items;
  };

}
