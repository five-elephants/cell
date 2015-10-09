#pragma once

#include "sim/llvm_namespace.h"
#include <ostream>
#include <string>
#include <unordered_set>
#include <regex>
#include <llvm/IR/DataLayout.h>

namespace sim {

  //
  // function prototypes
  //

  std::ostream& write_cpp(std::ostream& os, std::shared_ptr<Llvm_port> port);
  std::ostream& write_cpp(std::ostream& os, std::shared_ptr<Llvm_type> ty);


  //
  // function implementation
  //

  std::string cpp_name(std::string const& name) {
    std::regex re("\\.");
    std::stringstream strm;

    strm << std::regex_replace(name, re, "_");

    return strm.str();
  }


  std::ostream& write_cpp(std::ostream& os, std::shared_ptr<Llvm_port> port) {
    std::map<std::string,std::string> predef;

    predef["unit"] = "void";
    predef["void"] = "void";
    predef["int"] = "int64_t";
    predef["bool"] = "bool";
    predef["float"] = "double";

    os << "\t";
    if( predef.count(port->type->name) )
      os << predef[port->type->name];
    else
      os << port->type->name;
    os << " " << port->name << ";\n";
    return os;
  }


  std::ostream& write_cpp(std::ostream& os, std::shared_ptr<Llvm_type> ty) {
    std::stringstream buf;
    std::unordered_set<std::shared_ptr<Llvm_type>> deps;

    if( !ty->elements.empty() ) {
      buf << "struct " << ty->name << " {\n";
      for(auto const& elem : ty->elements) {
        deps.insert(elem.second->type);
        write_cpp(buf, elem.second);
      }
      buf << "};\n";
    }

    for(auto const& d : deps) {
      write_cpp(os, d);
    }
    os << buf.str();

    return os;
  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
