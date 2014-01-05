#pragma once

#include "ast/tree_base.h"
#include <vector>

namespace ast {

	class Module_def : public Tree_base {
		public:
			Module_def(Node_if& identifier);
			virtual ~Module_def() {};

			virtual void visit();

			void append(Node_if& node);
			void append(std::vector<Node_if*> const& nodes);

			Node_if& identifier() { return m_identifier; }
			Node_if const& identifier() const { return m_identifier; }

			bool has_socket() const { return (m_socket != nullptr); }
			void socket(Node_if& node) { m_socket = &node; }
			Node_if const& socket() const { return *m_socket; }

		private:
			Node_if& m_identifier;
			Node_if* m_socket;
			std::vector<Node_if*> m_elements;
	};

}


/* vim: set noet fenc=utf-8 ff=unix sts=0 sw=4 ts=4 : */
