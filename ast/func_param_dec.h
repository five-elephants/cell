#pragma once

#include "ast/variable_def.h"

namespace ast {

	class Func_param_dec : public Variable_def {
		public:
			Func_param_dec(Node_if& identifier);
			Func_param_dec(Node_if& identifier,
					Node_if& type,
					Node_if& expression);
			virtual ~Func_param_dec();

			virtual void set_generator(gen::Generator_if& g); 
			virtual void visit();
      virtual void visit(std::function<void(Node_if const&)> callback) const;
	};

}

