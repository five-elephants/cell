#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <boost/dynamic_bitset.hpp>

#include "time.h"

namespace ir {

  struct No_impl {
    struct Type {};
    struct Port {};
    struct Object {};
    struct Port_assignment {};
    struct Instantiation {};
    struct Function {};
    struct Process {};
    struct Periodic {};
    struct Once {};
    struct Socket {};
    struct Namespace {};
    struct Module {};
    struct Library {};
  };


  // prototypes
  template<typename Impl> struct Module;
  template<typename Impl> struct Namespace;
  template<typename Impl> struct Port;
  template<typename Impl> struct Library;

  typedef std::string Label;
  typedef boost::dynamic_bitset<> Bitset;

  template<typename Impl = No_impl>
  struct Type {
    Type()
      : enclosing_ns(nullptr) {
    }

    Type(Label _name)
      : name(_name),
        enclosing_ns(nullptr) {
    }

    //struct Field {
      //Label name;
      //std::shared_ptr<Type> type;
    //};

    Label name;
    Namespace<Impl>* enclosing_ns;
    //std::vector<std::shared_ptr<Field>> fields;
    std::map<Label, std::shared_ptr<Port<Impl>>> elements;
    std::size_t array_size = 1;

    typename Impl::Type impl;
  };

  template<typename Impl = No_impl>
  struct Object {
    std::shared_ptr<Type<Impl>> type;
    Label name;

    typename Impl::Object impl;
  };

  template<typename Impl = No_impl>
  struct Port_assignment {
    std::shared_ptr<Port<Impl>> port;
    std::shared_ptr<Object<Impl>> object;

    typename Impl::Port_assignment impl;
  };

  template<typename Impl = No_impl>
  struct Instantiation {
    Label name;
    std::shared_ptr<Module<Impl>> module;
    std::vector<std::shared_ptr<Port_assignment<Impl>> > connection;

    typename Impl::Instantiation impl;
  };

  template<typename Impl = No_impl>
  struct Function {
    Label name;
    std::shared_ptr<Type<Impl>> return_type;
    std::vector<std::shared_ptr<Object<Impl>> > parameters;
    bool within_module = false;

    typename Impl::Function impl;
  };

  template<typename Impl = No_impl>
  struct Operator {
    Label name;
    std::shared_ptr<Type<Impl>> return_type;
    std::shared_ptr<Type<Impl>> left, right;

    typename Impl::Operator impl;
  };

  template<typename Impl = No_impl>
  struct Process {
    std::shared_ptr<Function<Impl>> function;

    typename Impl::Process impl;
  };

  template<typename Impl = No_impl>
  struct Periodic : public Process<Impl> {
    Time period;

    typename Impl::Periodic impl;
  };

  template<typename Impl = No_impl>
  struct Once : public Process<Impl> {
    Time time;

    typename Impl::Once impl;
  };

  template<typename Impl = No_impl>
  struct Recurrent : public Process<Impl> {
    Label time_id;

    typename Impl::Recurrent impl;
  };

  enum class Direction {
    Input,
    Output,
    Bidirectional
  };

  template<typename Impl = No_impl>
  struct Port {
    Label name;
    Direction direction;
    std::shared_ptr<Type<Impl>> type;

    typename Impl::Port impl;
  };

  template<typename Impl = No_impl>
  struct Socket : public Type<Impl> {
    Socket()
      : Type<Impl>() {
    }

    Socket(Label name)
        : Type<Impl>(name) {
      this->name = name;
    }

    //Namespace<Impl>* enclosing_ns;
    std::map<Label, std::shared_ptr<Port<Impl>>> ports;

    typename Impl::Socket impl;
  };


  template<typename Impl = No_impl>
  struct Namespace {
    Namespace()
      : enclosing_ns(nullptr) {
    }

    Namespace(Label const& _name)
      : name(_name),
        enclosing_ns(nullptr) {
    }

    //Namespace(Namespace<No_impl> const& no_impl) {
      //name = no_impl.name;
      //enclosing_ns = no_impl.enclosing_ns;
    //}


    Label name;
    Namespace<Impl>* enclosing_ns;
    std::weak_ptr<Library<Impl>> enclosing_library;

    std::map<Label, std::shared_ptr<Module<Impl>>> modules;
    std::map<Label, std::shared_ptr<Namespace<Impl>>> namespaces;
    std::map<Label, std::shared_ptr<Type<Impl>>> sockets;
    std::map<Label, std::shared_ptr<Type<Impl>>> types;
    std::map<Label, std::shared_ptr<Function<Impl>>> functions;
    std::multimap<Label, std::shared_ptr<Operator<Impl>>> operators;

    typename Impl::Namespace impl;
  };


  template<typename Impl = No_impl>
  struct Module : public Namespace<Impl> {
    Module()
      : Namespace<Impl>() {
    }

    Module(Label const& label)
      : Namespace<Impl>(label) {
    }

    std::shared_ptr<Type<Impl>> socket;
    std::map<Label, std::shared_ptr<Object<Impl>>> objects;
    std::map<Label, std::shared_ptr<Instantiation<Impl>>> instantiations;
    std::vector<std::shared_ptr<Process<Impl>>> processes;
    std::vector<std::shared_ptr<Periodic<Impl>>> periodicals;
    std::vector<std::shared_ptr<Once<Impl>>> onces;
    std::vector<std::shared_ptr<Recurrent<Impl>>> recurrents;
    //std::shared_ptr<Codeblock_if> constructor_code;

    typename Impl::Module impl;
  };


  template<typename Impl = No_impl>
  struct Library {
    Label name;
    std::shared_ptr<Namespace<Impl>> ns;

    typename Impl::Library impl;
  };

}
