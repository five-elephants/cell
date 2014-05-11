#pragma once

#include "ast/tree_base.h"

namespace ast {
 
  class Process : public Tree_base {
    public:
      Process(Node_if& body);
    
      virtual void visit();
    
      Node_if const& body() const { return m_body; }
    
    private:
      Node_if& m_body;
  };
  
}
