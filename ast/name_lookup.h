#pragma once

namespace ast {

  class Name_lookup : public Tree_base {
    public:
      Name_lookup(Node_if& identifier)
        : Tree_base(),
          m_id(identifier) {
        register_branches({&m_id});
      }

      virtual void visit() {}

      Identifier const& identifier() const {
        return dynamic_cast<Identifier const&>(m_id);
      }

    protected:
      Node_if& m_id;

  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
