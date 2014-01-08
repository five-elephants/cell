#pragma once

#include "ast/variable_def.h"

namespace ast {

	class Func_param_dec : public Variable_def {
		public:
			Func_param_dec(Node_if& identifier);
			Func_param_dec(Node_if& identifier,
					Node_if& type,
					Node_if& expression);

			virtual void visit();
	};

}


/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
