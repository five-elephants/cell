#pragma once

#include "ast/ast.h"
#include "ir/serialize_shared_ptr.h"
#include <map>
#include <string>
#include <memory>
#include <boost/serialization/map.hpp>

namespace ir {

  // prototypes
  struct Module;
  struct Namespace;
  struct Port;

  typedef std::string Label;

  struct Type {
    Label name;
  };

  struct Object {
    std::shared_ptr<Type> type;
    Label name;
  };

  struct Port_assignment {
    std::shared_ptr<Port> port;
    std::shared_ptr<Object> object;
  };

  struct Instantiation {
    Label name;
    std::shared_ptr<Module> module;
    std::vector<std::shared_ptr<Port_assignment>> connection;
  };

  struct Function {
    std::shared_ptr<Type> return_type;
    Label name;
  };

  enum class Direction {
    Input,
    Output,
    Bidirectional
  };
  
  struct Port {
    Label name;
    Direction direction;
    std::shared_ptr<Type> type;
  };

  struct Socket : public Type {
    Socket(Label name);
    Socket(ast::Socket_def const& sock);
    
    Namespace* enclosing_ns;
    std::map<Label, std::shared_ptr<Port>> ports;

    void scan_ast(ast::Node_if const& tree);
  };


  struct Namespace {
    Namespace(Label const& _name)
      : name(_name),
        enclosing_ns(nullptr) {
    };

    Label name;
    Namespace* enclosing_ns;
    std::map<Label, std::shared_ptr<Module>> modules;
    std::map<Label, std::shared_ptr<Namespace>> namespaces;
    std::map<Label, std::shared_ptr<Socket>> sockets;
    std::map<Label, std::shared_ptr<Type>> types;
    std::map<Label, std::shared_ptr<Function>> functions;

    void insert_module(ast::Module_def const& mod);
    void insert_namespace(ast::Namespace_def const& ns);
    void insert_socket(ast::Socket_def const& sock);
    void insert_function(ast::Function_def const& func);

    void scan_ast(ast::Node_if const& tree);
  };

  struct Module : public Namespace {
    Module(ast::Module_def const& mod);

    std::shared_ptr<Socket> socket;
    std::map<Label, std::shared_ptr<Object>> objects;
    std::map<Label, std::shared_ptr<Instantiation>> instantiations;

    void insert_object(ast::Variable_def const& node);
    //void insert_instantiation(ast::Module_instantiation const& inst);

    void scan_ast(ast::Node_if const& tree);
  };

}
