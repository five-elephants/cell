#pragma once

#include "ast/tree_base.h"
#include "ast/identifier.h"
#include "ast/qualified_name.h"
#include "ast/table_def_item.h"
#include <vector>
#include <algorithm>

namespace ast {

  class Table_def : public Tree_base {
    public:
      Table_def(Node_if* identifier,
          Node_if* value_type_qname,
          std::vector<Node_if*>* items)
        : Tree_base(),
          m_value_type(value_type_qname),
          m_identifier(identifier) {
        m_items.resize(items->size());
        std::copy(items->begin(), items->end(), m_items.begin());

        register_branches({m_identifier, m_value_type});
        register_branch_lists({&m_items});
      }

      std::string name() const {
        return dynamic_cast<Identifier const&>(*m_identifier).identifier();
      }

      std::vector<std::string> value_type() const {
        return dynamic_cast<Qualified_name const&>(*m_value_type).name();
      }

      std::vector<std::pair<std::string,Node_if const*>> items() const {
        std::vector<std::pair<std::string,Node_if const*>> rv;
        for(auto const& i : m_items) {
          auto item = dynamic_cast<Table_def_item const&>(*i);
          rv.push_back(std::make_pair(item.name(), i));
        }
        return rv;
      }

    private:
      Node_if* m_identifier;
      Node_if* m_value_type;
      std::vector<Node_if*> m_items;
  };

}
