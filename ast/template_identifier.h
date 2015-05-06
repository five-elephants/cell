#pragma once

#include "ast/tree_base.h"
#include <vector>
#include <string>

namespace ast {

  class Template_identifier : public Tree_base {
    public:
      Template_identifier(std::string id, std::vector<Node_if*>* args)
        : m_identifier(id) {
        m_args.resize(args->size());
        std::copy(args->begin(), args->end(), m_args.begin());
      }

      virtual void visit() {}

      std::string const& identifier() const {
        return m_identifier;
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
      std::string m_identifier;
      std::vector<Node_if*> m_args;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
