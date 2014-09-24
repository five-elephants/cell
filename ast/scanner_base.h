#pragma once

#include "ast/visitor.h"

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <typeinfo>
#include <type_traits>

// some macros
#define SCANNER_ENTER_IF_TYPE(x, y) enter_if_type< x >([](decltype(*this)& scanner, ast::Node_if const& node) -> bool { y })


namespace ast {

  template<typename Base>
  class Scanner_base : public Visitor_base {
    public:
      typedef std::function<bool(Base&, ast::Node_if const& node)> Node_function;
      typedef std::unordered_multimap<std::type_info const*, Node_function> Node_type_map;
      typedef std::vector<Node_function> Node_function_list;

      Node_type_map m_mappings_enter;
      Node_type_map m_mappings_visit;
      Node_type_map m_mappings_leave;

      Node_function_list m_enter_callbacks;
      Node_function_list m_visit_callbacks;
      Node_function_list m_leave_callbacks;


      template<typename Node_type>
      void do_if_type(Node_type_map& map, Node_function func) {
        map.insert(std::make_pair(&typeid(Node_type), func));
      }

      template<typename Function>
      typename std::enable_if<std::is_same<typename std::remove_pointer<Function>::type, bool(Base&, ast::Node_if const&)>::value>::type
      do_always(Node_function_list& list, Function func) {
        list.push_back(func);
      }


      template<typename Function>
      typename std::enable_if<std::is_function<typename std::remove_pointer<Function>::type>::value>::type
      do_always(Node_function_list& list, 
          Function func) {
        Node_function wrapper = [func](Base&, ast::Node_if const&) -> bool {
          return func();
        };

        list.push_back(wrapper);
      }
      
      template<typename Node_type>
      void enter_if_type(Node_function func) {
        do_if_type<Node_type>(m_mappings_enter, func);
      }

      template<typename Node_type>
      void visit_if_type(Node_function func) {
        do_if_type<Node_type>(m_mappings_visit, func);
      }

      template<typename Node_type>
      void leave_if_type(Node_function func) {
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


      virtual bool process_node(Node_type_map const& map,
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



  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
