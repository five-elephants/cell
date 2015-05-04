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


    private:
      std::string m_identifier;
      std::vector<Node_if*> m_args;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
