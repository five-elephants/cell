#pragma once

#include "namespace.h"

#include <vector>
#include <sstream>
#include <stdexcept>

namespace ir {


  template<typename Impl, typename FuncIt, typename ParamIt>
  std::shared_ptr<Function<Impl>> resolve_function_overload(FuncIt const& func_a,
      FuncIt const& func_b,
      ParamIt param_type_a,
      ParamIt param_type_b) {
    std::vector<std::shared_ptr<Function<Impl>>> candidates;
    size_t num_params = std::distance(param_type_a, param_type_b);

    // find candidate functions with matching signature
    for(auto func_it = func_a; func_it != func_b; ++func_it) {
      std::shared_ptr<Function<Impl>> f = func_it->second;

      bool match = true;
      auto param_a = std::begin(f->parameters);
      auto param_b = std::end(f->parameters);

      if( num_params != f->parameters.size() )
        continue;

      for(auto pty_it=param_type_a; pty_it != param_type_b; ++pty_it) {
        if( (param_a == param_b) || (*pty_it != (*param_a)->type) ) {
          match = false;
          break;
        }

        ++param_a;
      }

      if( match )
        candidates.push_back(f);
    }


    if( candidates.size() > 1 ) {
      std::stringstream strm;
      strm << "Can not resolve overloaded function, because multiple candidates remain:\n";
      for(auto it=candidates.begin(); it != candidates.end(); ++it) {
        std::shared_ptr<Function<Impl>> func = *it;
        strm << "    "
          << "'" << func->name << "' (";
        for(auto para : func->parameters) {
          strm << para->name << " : " << para->type->name << ", ";
        }
        strm << ") -> "
          << func->return_type->name
          << '\n';
      }
      throw std::runtime_error(strm.str());
    } else if( candidates.empty() ) {
      return nullptr;
    }

    return candidates[0];
  }


}


/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
