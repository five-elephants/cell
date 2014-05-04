#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>

namespace ir {

  // prototypes
  struct Module;
  struct Namespace;
  struct Port;
  class Codeblock_if;

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
    std::vector<std::shared_ptr<Port_assignment> > connection;
  };

  struct Function {
    Label name;
    std::shared_ptr<Type> return_type;
    //std::map<Label, std::shared_ptr<Object>> parameters;
    std::vector<std::shared_ptr<Object>> parameters;
    std::shared_ptr<Codeblock_if> code;
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
    Socket() 
      : Type() {
    }

    Socket(Label name);
    
    Namespace* enclosing_ns;
    std::map<Label, std::shared_ptr<Port>> ports;
  };


  struct Namespace {
    Namespace() 
      : enclosing_ns(nullptr) {
    }

    Namespace(Label const& _name)
      : name(_name),
        enclosing_ns(nullptr) {
    }

    Label name;
    Namespace* enclosing_ns;
    std::map<Label, std::shared_ptr<Module>> modules;
    std::map<Label, std::shared_ptr<Namespace>> namespaces;
    std::map<Label, std::shared_ptr<Socket>> sockets;
    std::map<Label, std::shared_ptr<Type>> types;
    std::map<Label, std::shared_ptr<Function>> functions;
  };

  struct Module : public Namespace {
    Module() 
      : Namespace() {
    }

    Module(Label const& label)
      : Namespace(label) {
    }

    std::shared_ptr<Socket> socket;
    std::map<Label, std::shared_ptr<Object>> objects;
    std::map<Label, std::shared_ptr<Instantiation>> instantiations;
    std::shared_ptr<Codeblock_if> constructor_code;
  };

}
