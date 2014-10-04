#pragma once

#include "namespace.h"
#include "builtins.h"

namespace ir {

  template<typename T, typename Impl>
  std::shared_ptr<T> find_in_namespace(Namespace<Impl> const& ns,
      std::map<Label, std::shared_ptr<T>> Namespace<Impl>::*field,
      Label const& name) {
    // search in each namespace going up the hierarchy
    for(Namespace<Impl> const* cur_ns = &ns;
        cur_ns != nullptr;
        cur_ns = cur_ns->enclosing_ns) {
      auto m = cur_ns->*field;  // get the appropriate map to search in

      auto it = m.find(name);
      if( it != m.end() )
        return it->second;
    }

    return std::shared_ptr<T>(nullptr); 
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Socket<Impl>> find_socket(Namespace<Impl> const& ns,
      Label const& sock_name) {
    auto rv = find_in_namespace<Socket<Impl>>(ns, &Namespace<Impl>::sockets, sock_name);
    if( !rv ) {
      if( sock_name == Builtins<Impl>::null_socket->name ) {
        return Builtins<Impl>::null_socket;
      }
    }
    return rv;
  }
  

  template<typename Impl = No_impl>
  std::shared_ptr<Type<Impl>> find_type(Namespace<Impl> const& ns,
      Label const& type_name) {
    auto rv = find_in_namespace<Type<Impl>>(ns, &Namespace<Impl>::types, type_name);
    if( !rv ) {
      auto it = Builtins<Impl>::types.find(type_name);
      if( it != Builtins<Impl>::types.end() )
        return it->second;
    }
    return rv;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Function<Impl>> find_function(Namespace<Impl> const& ns,
      Label const& func_name) {
    auto rv = find_in_namespace<Function<Impl>>(ns, &Namespace<Impl>::functions, func_name);
    if( !rv ) {
      auto it = Builtins<Impl>::functions.find(func_name);
      if( it != Builtins<Impl>::functions.end() )
        return it->second;
    }
    return rv;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Module<Impl>> find_module(Namespace<Impl> const& ns,
      Label const& module_name) {
    auto rv = find_in_namespace<Module<Impl>>(ns, &Namespace<Impl>::modules, module_name);
    return rv;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Object<Impl>> find_object(Module<Impl> const& mod,
      Label const& object_name) {
    auto it = mod.objects.find(object_name);
    if( it != mod.objects.end() ) {
      return it->second;
    }

    return std::shared_ptr<Object<Impl>>(nullptr);
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Port<Impl>> find_port(Module<Impl> const& mod,
      Label const& port_name) {
    auto it = mod.socket->ports.find(port_name);
    if( it != mod.socket->ports.end() ) {
      return it->second;
    }

    return std::shared_ptr<Port<Impl>>(nullptr);
  }
}
