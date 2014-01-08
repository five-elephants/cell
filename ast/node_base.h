#pragma once

#include "ast/node_if.h"

namespace ast {

  class Node_base : public Node_if {
    public:
      static gen::Generator_if& default_generator;

      Node_base();

      virtual void visit(std::function<void(Node_if const&)> callback) const;
      virtual bool accept(Visitor_if& visitor) const;

      virtual void set_generator(gen::Generator_if& gen);
      virtual gen::Generator_if& get_generator();

      virtual void location(yy::location loc);
      virtual yy::location location() const;

    private:
      gen::Generator_if* m_generator;
      yy::location m_location;
  };

}
