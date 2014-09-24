#pragma once

#include "ast/visitor.h"

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <typeinfo>
#include <type_traits>


namespace ast {

  template<typename Base>
  class Scanner_base : public Visitor_base {
    private:
      typedef std::function<bool(Base&, ast::Node_if const& node)> Node_function;
      typedef std::unordered_multimap<std::type_info const*, Node_function> Node_type_map;
      typedef std::vector<Node_function> Node_function_list;

      Node_type_map m_mappings_enter;
      Node_type_map m_mappings_visit;
      Node_type_map m_mappings_leave;

      Node_function_list m_enter_callbacks;
      Node_function_list m_visit_callbacks;
      Node_function_list m_leave_callbacks;


      //
      // overloaded methods for type matching
      //

      template<typename Node_type, typename Function>
      typename std::enable_if<std::is_same<Function, bool(Base::*)(Node_type const&)>::value>::type
      do_if_type(Node_type_map& map, Function func) {
        Node_function wrapper = [func](Base& b, ast::Node_if const& n) -> bool {
          return (b.*func)(dynamic_cast<Node_type const&>(n));
        };

        map.insert(std::make_pair(&typeid(Node_type), wrapper));
      }



      //
      // overloaded methods for unconditional callbacks
      //


      // matching free functions (pointers)
      template<typename Function>
      typename std::enable_if<std::is_same<typename std::remove_pointer<Function>::type, bool(Base&, ast::Node_if const&)>::value>::type
      do_always(Node_function_list& list, Function func) {
        list.push_back(func);
      }


      // matching free functions without arguments
      template<typename Function>
      typename std::enable_if<std::is_same<typename std::remove_pointer<Function>::type, bool()>::value>::type
      do_always(Node_function_list& list, 
          Function func) {
        Node_function wrapper = [func](Base&, ast::Node_if const&) -> bool {
          return func();
        };

        list.push_back(wrapper);
      }


      // matching pointer to member functions
      template<typename Function>
      typename std::enable_if<std::is_same<Function, bool(Base::*)()>::value>::type
      do_always(Node_function_list& list, 
          Function func) {
        Node_function wrapper = [func](Base& b, ast::Node_if const&) -> bool {
          return (b.*func)();
        };

        list.push_back(wrapper);
      }


    public:
      //
      // nicely named functions for the user
      //

      template<typename Node_type, typename Function>
      void on_enter_if_type(Function func) {
        do_if_type<Node_type>(m_mappings_enter, func);
      }

      template<typename Node_type, typename Function>
      void on_visit_if_type(Function func) {
        do_if_type<Node_type>(m_mappings_visit, func);
      }

      template<typename Node_type, typename Function>
      void on_leave_if_type(Function func) {
        do_if_type<Node_type>(m_mappings_leave, func);
      }


      template<typename Function>
      void on_enter(Function func) {
        do_always(m_enter_callbacks, func);
      }

      void on_visit(Node_function func) {
        do_always(m_visit_callbacks, func);
      }

      void on_leave(Node_function func) {
        do_always(m_leave_callbacks, func);
      }

      //
      // overloaded functions from the base class for the visitor pattern
      //

      virtual bool visit(Node_if const& node) {
        return process_node(m_mappings_visit,
            m_visit_callbacks,
            node);
      }


      virtual bool enter(Node_if const& node) {
        return process_node(m_mappings_enter,
            m_enter_callbacks,
            node);
      }


      virtual bool leave(Node_if const& node) {
        return process_node(m_mappings_leave,
            m_leave_callbacks,
            node);
      }


    private:
      //
      // utility function for the visitor pattern to call the callbacks
      //
      
      bool process_node(Node_type_map const& map,
          Node_function_list& always_list,
          Node_if const& node) {
        bool rv = true;
        std::type_info const* node_type = &typeid(node);

        auto mappings = map.equal_range(node_type);
        for(auto mapping = mappings.first;
            mapping != mappings.second;
            ++mapping) {
          try {
            if( !mapping->second(*(static_cast<Base*>(this)), node) )
              rv = false;
          } catch( std::out_of_range const& err ) {
            std::cerr << node.location()
              << ": Encountered out_of_range error. Probably a missing "
                "value for a node of the AST.\n"
                "Node is of type: "
              << node_type->name()
              << "\nwhat(): " << err.what()
              << std::endl;
            throw err;
          }
        }

        for(auto cb : always_list) {
          if( !(cb)(*(static_cast<Base*>(this)), node) )
            rv = false;
        }

        return rv;
      }

  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
