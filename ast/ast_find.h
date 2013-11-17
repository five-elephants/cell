#pragma once

#include <typeinfo>

namespace ast {

  template<typename T>
  std::vector<T const*>
  find_by_type(Node_if const& tree) {
    std::vector<T const*> rv;
    auto filter = [&rv] (Node_if const& n) { 
      if( typeid(n) == typeid(T))
        rv.push_back(dynamic_cast<T const*>(&n));
    };

    tree.visit(filter);
    return rv;
  }

}
