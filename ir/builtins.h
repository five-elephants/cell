#pragma once

#include "namespace.h"

namespace ir {

  template<typename Impl = No_impl>
  struct Builtins {
    static std::map<Label, std::shared_ptr<Type<Impl>>> types;
    static std::map<Label, std::shared_ptr<Object<Impl>>> objects;
    static std::map<Label, std::shared_ptr<Function<Impl>>> functions;
    static std::multimap<Label, std::shared_ptr<Operator<Impl>>> operators;

    static std::shared_ptr<Socket<Impl>> null_socket;

    static void init() {
      auto printf = std::make_shared<Function<Impl>>();
      printf->name = "print";
      printf->return_type = types["int"];
      printf->parameters.emplace_back(new Object<Impl>{types["string"], "msg"});
      functions["print"] = printf;
    };
  };


  template<typename Impl>
	std::map<Label, std::shared_ptr<Type<Impl>>> Builtins<Impl>::types {
		{"void",		std::shared_ptr<Type<Impl>>(new Type<Impl>{"void"})},
		{"bool",		std::shared_ptr<Type<Impl>>(new Type<Impl>{"bool"})},
		{"int",			std::shared_ptr<Type<Impl>>(new Type<Impl>{"int"})},
		{"logic",		std::shared_ptr<Type<Impl>>(new Type<Impl>{"logic"})},
		{"string",		std::shared_ptr<Type<Impl>>(new Type<Impl>{"string"})}
	};

  template<typename Impl>
	std::map<Label, std::shared_ptr<Object<Impl>>> Builtins<Impl>::objects {
	};

  template<typename Impl>
	std::map<Label, std::shared_ptr<Function<Impl>>> Builtins<Impl>::functions {
	};

  template<typename Impl>
  std::multimap<Label, std::shared_ptr<Operator<Impl>>> Builtins<Impl>::operators {
  };


  template<typename Impl>
	std::shared_ptr<Socket<Impl>> Builtins<Impl>::null_socket(new Socket<Impl>("null_socket"));
}
