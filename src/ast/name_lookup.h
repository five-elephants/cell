#pragma once

#include <vector>
#include <algorithm>

namespace ast {

  class Name_lookup : public Tree_base {
    public:
      Name_lookup(std::vector<Node_if*> qname)
        : Tree_base() {
        m_qname.resize(qname.size());
        std::copy(qname.begin(), qname.end(), m_qname.begin());

        register_branch_lists({&m_qname});
      }

      std::vector<std::string> qname() const {
        std::vector<std::string> rv;
        for(auto const& i : m_qname) {
          rv.push_back(dynamic_cast<Identifier const&>(*i).identifier());
        }
        return rv;
      }


    protected:
      std::vector<Node_if*> m_qname;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
