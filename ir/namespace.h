#pragma once

#include "ast/ast.h"
#include "ir/serialize_shared_ptr.h"
#include <map>
#include <string>
#include <memory>
#include <boost/serialization/map.hpp>

namespace ir {

  typedef std::string Label;

  struct Type {
    Label name;

    template<typename Archive>
    void serialize(Archive& ar, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(name);
    }
  };

  struct Object {
    std::shared_ptr<Type> type;
    Label name;

    template<typename Archive>
    void serialize(Archive& ar, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(type);
      ar & BOOST_SERIALIZATION_NVP(name);
    }
  };

  struct Function {
    std::shared_ptr<Type> return_type;
    Label name;

    template<typename Archive>
    void serialize(Archive& ar, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(return_type);
      ar & BOOST_SERIALIZATION_NVP(name);
    }
  };

  struct Module {
    Module(ast::Module_def const& mod);

    std::map<Label, std::shared_ptr<Type>> types;
    std::map<Label, std::shared_ptr<Object>> objects;
    std::map<Label, std::shared_ptr<Function>> functions;
    Label name;

    void scan_ast(ast::Node_if const& tree);

    template<typename Archive>
    void serialize(Archive& ar, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(types);
      ar & BOOST_SERIALIZATION_NVP(objects);
      ar & BOOST_SERIALIZATION_NVP(functions);
    }
  };

  struct Namespace {
    Namespace(Label const& _name)
      : name(_name) {
    };

    Label name;
    std::map<Label, std::shared_ptr<Module>> modules;

    void insert_module(ast::Module_def const& mod);

    template<typename Archive>
    void serialize(Archive& ar, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(name);
      ar & BOOST_SERIALIZATION_NVP(modules);
    }
  };

  //--------------------------------------------------------------------------------
  // Free functions
  //--------------------------------------------------------------------------------

  extern std::shared_ptr<Type> find_type(Module const& m, Label type_name);

}
