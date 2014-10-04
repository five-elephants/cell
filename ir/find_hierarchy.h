#pragma once

#include "ir/namespace.h"

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <regex>
#include <algorithm>
#include <array>

namespace ir {

  inline std::vector<std::string> parse_path(std::string const& path,
      std::string const& separator = ".") {
    std::vector<std::string> rv;

    if( path.size() == 0 )
      return rv;

    // check for invalid characters
    static std::array<char, 4> const invalid_chars {{
      ' ', '\n', '\t', '\r'
    }};

    for(auto c : invalid_chars) {
      if( path.find(c) != std::string::npos )
        throw std::runtime_error("invalid character in path");
    }

    // check for leading and trailing separators

    auto tail = path;
    size_t pos;

    do {
      pos = tail.find(separator);
      rv.push_back(tail.substr(0, pos));
      tail = tail.substr(std::min(pos+separator.size(), tail.size()));
    } while( pos != std::string::npos );

    return rv;
  }
 

  template<typename T, typename Impl>
  std::shared_ptr<T> find_by_path(Namespace<Impl> const& ns,
      std::map<Label, std::shared_ptr<T>> Namespace<Impl>::*field,
      std::string const& path) {
    auto path_elems = parse_path(path, ".");

    std::cout << "PATH: " << path << "\n";
    Namespace<Impl> const* cur_ns = &ns;
    for(auto it=path_elems.begin();
        (path_elems.size() > 1) && (it != --(path_elems.end()));
        ++it) {
      auto i = *it;
      std::cout << "   '" << i << "'\n";
      
      if( cur_ns->namespaces.count(i) )
        cur_ns = cur_ns->namespaces.at(i).get();
      else if( cur_ns->modules.count(i) )
        cur_ns = cur_ns->modules.at(i).get();
      else
        return std::shared_ptr<T>(nullptr);
    } 

    auto m = cur_ns->*field;

    if( m.count(path_elems.back()) )
      return m[path_elems.back()];

    return std::shared_ptr<T>(nullptr);
  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
