#pragma once

#include "ast/tree_base.h"
#include "ast/qualified_name.h"
#include <string>
#include <vector>


namespace ast {

  class Constant_ref : public Tree_base {
    public:
      Constant_ref(Node_if& name)
        : Tree_base(),
          m_name(name) {
        register_branches({&m_name});
      }

      std::vector<std::string> name() const {
        return dynamic_cast<Qualified_name const&>(m_name).name();
      }

    private:
      Node_if& m_name;
  };

}


/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
