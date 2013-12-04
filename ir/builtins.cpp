#include "builtins.h"

namespace ir {

	std::map<Label, std::shared_ptr<Type>> Builtins::types {
		{"void",		std::shared_ptr<Type>(new Type{"void"})},
		{"int",			std::shared_ptr<Type>(new Type{"int"})}
	};

	std::map<Label, std::shared_ptr<Object>> Builtins::objects {
	};

	std::map<Label, std::shared_ptr<Function>> Builtins::functions {
	};

}
