#pragma once

namespace ir {

  inline std::vector<std::string> parse_path(std::string const& path,
      std::string const& separator = ".") {
    std::vector<std::string> rv;
 
    auto tail = path;
   
    while( tail.size() > 0 ) {
      auto pos = tail.find(separator);
      rv.push_back(tail.substr(0, pos));
      tail = tail.substr(pos+separator.size());
      if( pos == std::string::npos )
        break;
    }

    return rv;
  }
 

  template<typename T>
  std::shared_ptr<T> find_by_path(Namespace const& ns,
      std::map<Label, std::shared_ptr<T>> Namespace::*field,
      std::string const& path) {
    auto path_elems = parse_path(path, ".");

    std::cout << "PATH: " << path << "\n";
    for(auto i : path_elems) {
      std::cout << "   '" << i << "'\n";
    } 

    return std::shared_ptr<T>(nullptr);
  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
