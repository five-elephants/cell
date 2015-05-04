#pragma once

#include "ast/tree_base.h"
#include <vector>

namespace ast {

  class Module_template : public Tree_base {
    public:
      Module_template(std::vector<Node_if*>* args, Node_if* module_def);

      virtual void visit() {}

    private:
      std::vector<Node_if*> m_args;
      Node_if* m_def;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
