#pragma once

#include "ast/node_if.h"
#include "ast/identifier.h"
#include "ast/tree_base.h"
#include <vector>


namespace ast {

  class Qualified_name : public Tree_base {
    public:
      Qualified_name(Node_if* base_id) {
        m_ids.push_back(base_id);

        register_branch_lists({&m_ids});
      }


      void append(Node_if* id) {
        m_ids.push_back(id);
      }


			std::vector<std::string> name() const {
				std::vector<std::string> rv;
				for(auto const& n : m_ids)
					rv.push_back(dynamic_cast<Identifier const&>(*n).identifier());
				return rv;
			}


    private:
      std::vector<Node_if*> m_ids;

  };

}
