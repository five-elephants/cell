#pragma once

#include "namespace.h"
#include "builtins.h"
#include "resolve_function_overload.h"

#include <stdexcept>
#include <sstream>
#include <stdexcept>

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


  template<typename T, typename Impl>
  std::vector<typename std::multimap<Label, std::shared_ptr<T>>::value_type>
  find_in_namespace(Namespace<Impl> const& ns,
      std::multimap<Label, std::shared_ptr<T>> Namespace<Impl>::*field,
      Label const& name) {
    std::vector<typename std::multimap<Label, std::shared_ptr<T>>::value_type> rv;

    // search in each namespace going up the hierarchy
    for(Namespace<Impl> const* cur_ns = &ns;
        cur_ns != nullptr;
        cur_ns = cur_ns->enclosing_ns) {
      auto m = cur_ns->*field;  // get the appropriate map to search in

      if( m.count(name) > 0 ) {
        /* Bug in stdlib?
         * m.end() contains an uninitialized shared_ptr as second member. This
         * seems to be converted into a null-pointer when assigning. But then
         * it does no longer compare equal to m.end(). So copying the
         * equal_range pair makes it unusable as iterator range.
         * -> Fix: copy to std::vector and return that instead of the
         *  iterators. */
        auto range = m.equal_range(name);
        for(auto i=range.first; i != range.second; ++i)
          rv.push_back(*i);
        return rv;
      }
    }

    return rv;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Type<Impl>> find_socket(Namespace<Impl> const& ns,
      Label const& sock_name) {
    auto rv = find_in_namespace<Type<Impl>>(ns, &Namespace<Impl>::sockets, sock_name);
    if( !rv ) {
      if( sock_name == "unit" ) {
        return Builtins<Impl>::types.at("unit");
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
      else
        return std::shared_ptr<Type<Impl>>(nullptr);
    }
    return rv;
  }


  template<typename It, typename Impl = No_impl>
  std::shared_ptr<Function<Impl>> find_function(Namespace<Impl> const& ns,
      Label const& func_name,
      It param_type_a,
      It param_type_b) {
    auto const candidates = find_in_namespace<Function<Impl>>(ns,
        &Namespace<Impl>::functions,
        func_name);
    // It is probably not necessary to check this
    //if( (range.first == ns.functions.end())
        //|| (range.second == ns.functions.end()) ) {
      //std::stringstream strm;
      //strm << "Can not find function '"
        //<< func_name
        //<< "' starting at namespace '"
        //<< ns.name
        //<< "' (" << __func__ << ")";
      //throw std::runtime_error(strm.str());
    //}
    auto func = resolve_function_overload<Impl>(std::begin(candidates),
        std::end(candidates),
        param_type_a,
        param_type_b);
    if( !func ) {
      // check builtins
      auto const range2 = Builtins<Impl>::functions.equal_range(func_name);
      func = resolve_function_overload<Impl>(range2.first,
          range2.second,
          param_type_a,
          param_type_b);
      if( !func )
        return nullptr;
    }

    return func;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Module<Impl>> find_module(Namespace<Impl> const& ns,
      Label const& module_name) {
    auto rv = find_in_namespace<Module<Impl>>(ns, &Namespace<Impl>::modules, module_name);
    return rv;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Module_template<Impl>> find_module_template(Namespace<Impl> const& ns,
      Label const& module_name) {
    auto rv = find_in_namespace<Module_template<Impl>>(ns,
        &Namespace<Impl>::module_templates,
        module_name);
    return rv;
  }


  template<typename Impl = No_impl, typename Name>
  std::shared_ptr<Constant<Impl>> find_constant(Namespace<Impl> const& ns,
      Name const& const_name) {
    auto rv = find_in_namespace<Constant<Impl>>(ns, &Namespace<Impl>::constants,
        const_name);
    return rv;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Namespace<Impl>> find_namespace(Namespace<Impl> const& ns,
      Label const& ns_name) {
    auto rv = find_in_namespace<Namespace<Impl>>(ns,
        &Namespace<Impl>::namespaces,
        ns_name);
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
    auto it = mod.socket->elements.find(port_name);
    if( it != mod.socket->elements.end() ) {
      return it->second;
    }

    return std::shared_ptr<Port<Impl>>(nullptr);
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Library<Impl>> find_library(Namespace<Impl> const& ns) {
    auto lib = ns.enclosing_library.lock();
    if( !lib )
      throw std::runtime_error("Failed to get enclosing library");

    return lib;
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Operator<Impl>> find_operator(Namespace<Impl> const& ns,
      Label name,
      std::shared_ptr<Type<Impl>> return_type,
      std::shared_ptr<Type<Impl>> left,
      std::shared_ptr<Type<Impl>> right) {

    // search in each namespace going up the hierarchy
    for(Namespace<Impl> const* cur_ns = &ns;
        cur_ns != nullptr;
        cur_ns = cur_ns->enclosing_ns) {
      auto& m = cur_ns->operators;

      auto range = m.equal_range(name);
      if( range.first != m.end() ) {
        for(auto it = range.first; it != range.second; ++it) {
          auto& op = it->second;
          if( (op->return_type == return_type)
              && (op->left == left)
              && (op->right == right) ) {
            return op;
          }
        }
      }
    }

    // not found yet, check built-in operators
    {
      auto& m = Builtins<Impl>::operators;

      auto range = m.equal_range(name);
      if( range.first != m.end() ) {
        for(auto it = range.first; it != range.second; ++it) {
          auto& op = it->second;
          if( (op->return_type == return_type)
              && (op->left == left)
              && (op->right == right) ) {
            return op;
          }
        }
      }
    }

    // nothing found
    return std::shared_ptr<Operator<Impl>>(nullptr);
  }


  template<typename Impl = No_impl>
  std::shared_ptr<Operator<Impl>> find_operator(Namespace<Impl> const& ns,
      Label name,
      std::shared_ptr<Type<Impl>> left,
      std::shared_ptr<Type<Impl>> right) {

    // search in each namespace going up the hierarchy
    for(Namespace<Impl> const* cur_ns = &ns;
        cur_ns != nullptr;
        cur_ns = cur_ns->enclosing_ns) {
      auto& m = cur_ns->operators;

      auto range = m.equal_range(name);
      if( range.first != m.end() ) {
        for(auto it = range.first; it != range.second; ++it) {
          auto& op = it->second;
          if( (op->left == left) && (op->right == right) ) {
            return op;
          }
        }
      }
    }

    // not found yet, check built-in operators
    {
      auto& m = Builtins<Impl>::operators;

      auto range = m.equal_range(name);
      if( range.first != m.end() ) {
        for(auto it = range.first; it != range.second; ++it) {
          auto& op = it->second;
          if( (op->left == left) && (op->right == right) ) {
            return op;
          }
        }
      }
    }

    // nothing found
    return std::shared_ptr<Operator<Impl>>(nullptr);
  }

}
