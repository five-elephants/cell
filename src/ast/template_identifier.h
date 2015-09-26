#pragma once

#include "ast/tree_base.h"
#include "ast/node_if.h"
#include <vector>
#include <string>

namespace ast {

  class Template_identifier : public Tree_base {
    public:
      Template_identifier(std::vector<Node_if*>* qname,
          std::vector<Node_if*>* args)
        : Tree_base() {
        m_qname.resize(qname->size());
        std::copy(qname->begin(), qname->end(), m_qname.begin());
        m_args.resize(args->size());
        std::copy(args->begin(), args->end(), m_args.begin());
      }

      std::vector<std::string> name() const {
        std::vector<std::string> rv;
        for(auto const& n : m_qname) {
          auto tmp = dynamic_cast<ast::Identifier const&>(*n);
          rv.push_back(tmp.identifier());
        }
        return rv;
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
      std::vector<Node_if*> m_qname;
      std::vector<Node_if*> m_args;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
