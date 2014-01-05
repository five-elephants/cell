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
  };

  struct Object {
    std::shared_ptr<Type> type;
    Label name;
  };

  struct Function {
    std::shared_ptr<Type> return_type;
    Label name;
  };

  struct Module {
    Module(ast::Module_def const& mod);

    std::map<Label, std::shared_ptr<Type>> types;
    std::map<Label, std::shared_ptr<Object>> objects;
    std::map<Label, std::shared_ptr<Function>> functions;
    Label name;

    void scan_ast(ast::Node_if const& tree);
  };

  struct Namespace {
    Namespace(Label const& _name)
      : name(_name) {
    };

    Label name;
    std::map<Label, std::shared_ptr<Module>> modules;
    std::map<Label, std::shared_ptr<Namespace>> namespaces;

    void insert_module(ast::Module_def const& mod);
    void insert_namespace(ast::Namespace_def const& ns);

    void scan_ast(ast::Node_if const& tree);
  };

  //--------------------------------------------------------------------------------
  // Free functions
  //--------------------------------------------------------------------------------

  extern std::shared_ptr<Type> find_type(Module const& m, Label type_name);

}
