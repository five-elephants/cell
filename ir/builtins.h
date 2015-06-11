#pragma once

#include "namespace.h"

namespace ir {

  template<typename Impl = No_impl>
  struct Builtins {
    static std::map<Label, std::shared_ptr<Type<Impl>>> types;
    static std::map<Label, std::shared_ptr<Object<Impl>>> objects;
    static std::multimap<Label, std::shared_ptr<Function<Impl>>> functions;
    static std::multimap<Label, std::shared_ptr<Operator<Impl>>> operators;

    static std::shared_ptr<Socket<Impl>> null_socket;

    static void init() {
      types.clear();
      objects.clear();
      functions.clear();
      operators.clear();

      // types
      types["unit"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"unit"});
		  types["void"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"void"});
		  types["bool"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"bool"});
		  types["int"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"int"});
		  types["logic"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"logic"});
		  types["string"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"string"});
      types["float"] = std::shared_ptr<Type<Impl>>(new Type<Impl>{"float"});

      // functions
      auto printf = std::make_shared<Function<Impl>>();
      printf->name = "print";
      printf->return_type = types.at("int");
      printf->parameters.emplace_back(new Object<Impl>{types.at("string"),
          "msg"});
      functions.insert(std::make_pair("print", printf));

      auto randf = std::make_shared<Function<Impl>>();
      randf->name = "print";
      randf->return_type = types.at("int");
      functions.insert(std::make_pair("rand", randf));
    };
  };


  template<typename Impl>
	std::map<Label, std::shared_ptr<Type<Impl>>> Builtins<Impl>::types {
	};

  template<typename Impl>
	std::map<Label, std::shared_ptr<Object<Impl>>> Builtins<Impl>::objects {
	};

  template<typename Impl>
	std::multimap<Label, std::shared_ptr<Function<Impl>>> Builtins<Impl>::functions {
	};

  template<typename Impl>
  std::multimap<Label, std::shared_ptr<Operator<Impl>>> Builtins<Impl>::operators {
  };


  //template<typename Impl>
	//std::shared_ptr<Type<Impl>> Builtins<Impl>::null_socket(new Type<Impl>("null_socket"));

}
