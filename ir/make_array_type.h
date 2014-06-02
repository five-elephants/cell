#pragma once

#include "ast/ast.h"
#include "namespace.h"
#include "find.hpp"

#include <memory>
#include <stdexcept>
#include <sstream>


namespace ir {

  inline std::shared_ptr<Type> make_array_type(Namespace const& ns, ast::Array_type const& type) {
    if( typeid(type.base_type()) == typeid(ast::Array_type) ) {
      auto& base_type = dynamic_cast<ast::Array_type const&>(type.base_type());
      auto bt = make_array_type(ns, base_type);

      std::stringstream strm;
      strm << bt->name << "[" << type.size() << "]";
      auto new_type = std::make_shared<Type>();
      new_type->name = strm.str();
      new_type->array_size = type.size();

      return new_type;
    } else if( typeid(type.base_type()) == typeid(ast::Identifier) ) {
      auto& base_type = dynamic_cast<ast::Identifier const&>(type.base_type());
      auto type_name = base_type.identifier();
      auto ir_type = find_type(ns, type_name);

      if( !ir_type ) {
        std::stringstream strm;
        strm << type.location()
          << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }

      std::stringstream strm;
      strm << ir_type->name << "[" << type.size() << "]";
      auto new_type = std::make_shared<Type>();
      new_type->name = strm.str();
      new_type->array_size = type.size();

      return new_type;
    } else {
      throw std::runtime_error("Array base type is neither array nor identifier.");
    }
  }

}
