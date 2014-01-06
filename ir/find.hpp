#pragma once

#include "namespace.h"
#include "builtins.h"

namespace ir {

  template<typename T>
  std::shared_ptr<T> find_in_namespace(Namespace const& ns,
      std::map<Label, std::shared_ptr<T>> Namespace::*field,
      Label const& name) {
    // search in each namespace going up the hierarchy
    for(Namespace const* cur_ns = &ns;
        cur_ns != nullptr;
        cur_ns = cur_ns->enclosing_ns) {
      auto m = cur_ns->*field;  // get the appropriate map to search in

      auto it = m.find(name);
      if( it != m.end() )
        return it->second;
    }

    return std::shared_ptr<T>(nullptr); 
  }


  inline std::shared_ptr<Socket> find_socket(Namespace const& ns,
      Label const& sock_name) {
    auto rv = find_in_namespace<Socket>(ns, &Namespace::sockets, sock_name);
    if( !rv ) {
      if( sock_name == Builtins::null_socket->name ) {
        return Builtins::null_socket;
      }
    }
    return rv;
  }
  

  inline std::shared_ptr<Type> find_type(Namespace const& ns,
      Label const& type_name) {
    auto rv = find_in_namespace<Type>(ns, &Namespace::types, type_name);
    if( !rv ) {
      auto it = Builtins::types.find(type_name);
      if( it != Builtins::types.end() )
        return it->second;
    }
    return rv;
  }


}
