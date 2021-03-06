#include "module_template.h"


namespace ast {

  Module_template::Module_template(std::vector<Node_if*>* args, Node_if* def)
      : Tree_base() {
    m_def = def;
    m_args.resize(args->size());
    std::copy(args->begin(), args->end(), m_args.begin());

    register_branches({m_def});
    register_branch_lists({&m_args});
  }

}

/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
