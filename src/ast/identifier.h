#pragma once

#include "ast/node_base.h"
#include <string>

namespace ast {

	class Identifier : public Node_base {
		public:
			Identifier(std::string const& txt);

			std::string const& identifier() const { return m_identifier; }
			void identifier(std::string const& identifier) { m_identifier = identifier; }


		private:
			std::string m_identifier;
	};

}
