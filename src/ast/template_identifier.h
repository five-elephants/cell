#pragma once

#include "ast/tree_base.h"
#include "ast/node_if.h"
#include "ast/qualified_name.h"
#include <vector>
#include <string>

namespace ast {

  class Template_identifier : public Tree_base {
    public:
      Template_identifier(Node_if* qname,
          std::vector<Node_if*>* args)
        : Tree_base(),
          m_qname(qname) {
        m_args.resize(args->size());
        std::copy(args->begin(), args->end(), m_args.begin());

        register_branches({m_qname});
        register_branch_lists({&m_args});
      }

      std::vector<std::string> name() const {
        return dynamic_cast<Qualified_name const&>(*m_qname).name();
      }

      std::vector<Node_if*> const& args() const {
        return m_args;
      }

      std::vector<std::string> arg_type_names() const {
        std::vector<std::string> rv;
        rv.reserve(m_args.size());

        for(auto const& arg : m_args) {
          auto id = dynamic_cast<Identifier const&>(*arg);
          rv.push_back(id.identifier());
        }

        return rv;
      }

    private:
      Node_if* m_qname;
      std::vector<Node_if*> m_args;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
