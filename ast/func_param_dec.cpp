#include "ast/func_param_dec.h"

namespace ast {

	Func_param_dec::Func_param_dec(Node_if& identifier)
		:	Variable_def(identifier) {
	}

	Func_param_dec::Func_param_dec(Node_if& identifier,
			Node_if& type,
			Node_if& expression)
		:	
			Variable_def(identifier, type, expression) {
	}

	void
	Func_param_dec::visit() {
		get_generator().func_param_dec(*this);
	}

}

