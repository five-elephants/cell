#pragma once

#include "ast/tree_base.h"
#include "ast/identifier.h"


namespace ast {

  class Table_def_item : public Tree_base {
    public:
      Table_def_item(Node_if* identifier, Node_if* value)
        : Tree_base(),
          m_identifier(identifier),
          m_value(value) {
        register_branches({m_identifier, m_value});
      }

      virtual void visit() {}

      std::string name() const {
        return dynamic_cast<Identifier const&>(*m_identifier).identifier();
      }

      Node_if const* value() const {
        return m_value;
      }


    private:
      Node_if* m_identifier;
      Node_if* m_value;
  };

}
