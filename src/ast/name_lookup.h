#pragma once

#include "ast/qualified_name.h"
#include <vector>
#include <algorithm>

namespace ast {

  class Name_lookup : public Tree_base {
    public:
      Name_lookup(Node_if* qname)
        : Tree_base(),
          m_qname(qname) {
        register_branches({m_qname});
      }

      std::vector<std::string> qname() const {
        return dynamic_cast<Qualified_name const&>(*m_qname).name();
      }


    protected:
      Node_if* m_qname;
  };

}

/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
