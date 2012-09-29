#pragma once

#include "ast/node_if.h"

namespace ast {

	class Node_base : public Node_if {
		public:
			static gen::Generator_if& default_generator;

			Node_base();
			virtual ~Node_base() {};

			virtual void set_generator(gen::Generator_if& gen);
			virtual gen::Generator_if& get_generator();

		private:
			gen::Generator_if* m_generator;
	};

}
