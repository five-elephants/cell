#pragma once

#include "scanner_base.h"


namespace ast {

  class Scanner_chain : public Visitor_base {
      Visitor_base& before;
      Visitor_base& after;  


    public:
      Scanner_chain(Visitor_base& before, Visitor_base& after)
        : before(before),
          after(after) {
      }


      //
      // overloaded functions from the base class for the visitor pattern
      //

      virtual bool visit(Node_if const& node) {
        bool a, b;

        a = before.visit(node);
        b = after.visit(node);

        return a || b;
      }


      virtual bool enter(Node_if const& node) {
        bool a, b;

        a = before.enter(node);
        b = after.enter(node);

        return a || b;
      }


      virtual bool leave(Node_if const& node) {
        bool a, b;

        a = before.leave(node);
        b = after.leave(node);

        return a || b;
      }
  };  

}
