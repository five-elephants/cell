#include "builtins.h"

namespace ir {

	std::map<Label, std::shared_ptr<Type>> Builtins::types {
		{"void",		std::shared_ptr<Type>(new Type{"void"})},
		{"int",			std::shared_ptr<Type>(new Type{"int"})},
		{"logic",		std::shared_ptr<Type>(new Type{"logic"})},
		{"string",		std::shared_ptr<Type>(new Type{"string"})}
	};

	std::map<Label, std::shared_ptr<Object>> Builtins::objects {
	};

	std::map<Label, std::shared_ptr<Function>> Builtins::functions {
	};


	std::shared_ptr<Socket> Builtins::null_socket(new Socket("null_socket"));


	void Builtins::init() {
		auto printf = std::make_shared<Function>();
		printf->name = "print";
		printf->return_type = types["int"];
		printf->parameters.emplace_back(new Object{types["string"], "msg"});
		functions["print"] = printf;
	}

}
