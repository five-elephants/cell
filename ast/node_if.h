#pragma once

namespace gen {
	class Generator_if;
}

namespace ast {
	class Node_if {
		public:
			virtual void visit() = 0;
			virtual void set_generator(gen::Generator_if& gen) = 0;
			virtual gen::Generator_if& get_generator() = 0;
	};
}


