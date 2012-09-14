#pragma once

#include "ast/node_base.h"
#include <string>

namespace ast {

	class Identifier : public Node_base {
		public:
			Identifier(std::string const& txt, gen::Generator_if& generator)
				: Node_base(generator),
					m_identifier(txt) {
			}

			std::string const& identifier() const { return m_identifier; }
			void identifier(std::string const& identifier) { m_identifier = identifier; }

			virtual void visit() {
				get_generator().identifier(*this);
			}

		private:
			std::string m_identifier;
	};

}
