#pragma once

namespace ast {

  class Visitor_if {
    public:
      virtual bool visit(Node_if const& node) = 0;
      virtual bool enter(Node_if const& node) = 0;
      virtual bool leave(Node_if const& node) = 0;
  };


  class Visitor_base : public Visitor_if {
    public:
      virtual ~Visitor_base() {}

      virtual bool visit(Node_if const& node) {
        return true;
      }

      virtual bool enter(Node_if const& node) {
        return true;
      }

      virtual bool leave(Node_if const& node) {
        return true;
      }
  };

}
