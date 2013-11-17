#pragma once

#include "ast/node_base.h"

#include <tuple>
#include <vector>

namespace ast {

  template<typename Derived>
  class Tree_base : public Node_base {
    private:
      template<typename Tuple, int n, typename Callback>
      struct Visit_helper {
        void operator () (Tuple const& t, Callback callback) {
          auto& elem = std::get<n-1>(t);
          Do_visit<typename std::tuple_element<n-1, Tuple>::type, Callback> do_visit;
          Visit_helper<Tuple, n-1, Callback> visit_helper;

          do_visit(elem, callback);
          visit_helper(t, callback);
        }
      };

      template<typename Tuple, typename Callback>
      struct Visit_helper<Tuple, 0, Callback> {
        void operator () (Tuple const& t, Callback callback) {
        }
      };

      template<typename T, typename Callback>
      struct Do_visit {
        void operator () (T& branch, Callback callback) {
          branch.visit(callback);
        }
      };

      template<typename Callback>
      struct Do_visit<std::vector<Node_if*>, Callback> {
        void operator () (std::vector<Node_if*>& branch,
            std::function<void(Node_if const&)> callback) {
          for(auto i : branch) {
            i->visit(callback);
          }
        }
      };

    public:
      Tree_base()
        : Node_base() {
      }
      virtual ~Tree_base() {}

      //virtual void visit() {
      //}

      virtual void visit(std::function<void(Node_if const&)> callback) const {
        Node_base::visit(callback);
        auto& branches = static_cast<Derived const*>(this)->m_branches;
        Visit_helper<decltype(branches),
            std::tuple_size<decltype(static_cast<Derived const*>(this)->m_branches)>::value,
            decltype(callback)> visit_helper;
        visit_helper(branches, callback);
      }

      //virtual void set_generator(gen::Generator_if& gen) {
      //}
  };
}
