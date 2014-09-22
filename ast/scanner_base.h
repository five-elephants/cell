#pragma once

#include "ast/visitor.h"

#include <unordered_map>
#include <functional>
#include <typeinfo>

namespace ast {

  template<typename Base>
  class Scanner_base : public Visitor_base {
    public:
      typedef std::function<bool(Base&, ast::Node_if const& node)> Node_function;
      typedef std::unordered_multimap<std::type_info const*, Node_function> Node_type_map;

      Node_type_map m_mappings_enter;
      Node_type_map m_mappings_visit;
      Node_type_map m_mappings_leave;

      
      template<typename Node_type>
      void enter_if_type(Node_function func) {
        m_mappings_enter.insert(std::make_pair(&typeid(Node_type), func));
      }


      virtual bool visit(Node_if const& node) {
        return false;
      }


      virtual bool enter(Node_if const& node) {
        bool rv = true;
        std::type_info const* node_type = &typeid(node);

        auto mappings = m_mappings_enter.equal_range(node_type);
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

        return rv;
      }


      virtual bool leave(Node_if const& node) {
        return false;
      }



  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
