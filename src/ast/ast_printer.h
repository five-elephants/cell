#pragma once

#include "ast/visitor.h"

#include <ostream>


namespace ast {

  class Ast_printer : public Visitor_base {
    public:
      Ast_printer(std::ostream& os)
        : m_os(os) {
        m_indent = 0;
      }


      //
      // overloaded functions from the base class for the visitor pattern
      //
    
      /*  -[root]
       *   +-[node1]
       *   +-[node2]
       *   +-[node3]
       *   | +-[sub-node1]
       *   | +-[sub-node2]
       *   +-[node4]
       *  */

      virtual bool visit(Node_if const& node) {
        m_os << indent()
          << "-["
          << typeid(node).name()
          << "]\n";
        return true;
      }


      virtual bool enter(Node_if const& node) {
        m_os << indent()
          << "-["
          << typeid(node).name()
          << "]\n";
        ++m_indent;
        return true;
      }


      virtual bool leave(Node_if const& node) {
        --m_indent;
        return true;
      }



    private:
      std::ostream& m_os;
      int m_indent;


      std::string indent() {
        if( m_indent == 0 )
          return std::string("*");
        else {
          std::string rv;
          for(int i=0; i<m_indent; i++)
            rv += std::string("| ");
          rv += std::string("+");
          return rv;
        }
      }
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
