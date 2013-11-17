#pragma once

#include "ast/node_base.h"

namespace ast {

	class Variable_def : public Node_base {
		public:
			Variable_def(Node_if& identifier);
			Variable_def(Node_if& identifier,
					Node_if& type,
					Node_if& expression);
			virtual ~Variable_def();

			virtual void set_generator(gen::Generator_if& g); 
			virtual void visit();
      virtual void visit(std::function<void(Node_if const&)> cb) const;

			Node_if& identifier() { return m_identifier; }
			Node_if const& identifier() const { return m_identifier; }
			Node_if& type() { return *m_type; }
			Node_if const& type() const { return *m_type; }
			void type(Node_if& node) { 
				if( m_is_type_owner )
					delete m_type;
				m_type = &node;
			}
			Node_if& expression() { return *m_expression; }
			void expression(Node_if& node) { 
				if( m_is_expression_owner )
					delete m_expression;
				m_expression = &node;
			}

			bool without_expression() const { return m_is_expression_owner; }

		private:
			Node_if& m_identifier;
			Node_if* m_type;
			bool m_is_type_owner = false;
			Node_if* m_expression;
			bool m_is_expression_owner = false;

	};

}

