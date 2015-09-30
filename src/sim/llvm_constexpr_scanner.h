#pragma once

#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "ir/namespace.h"
#include "ir/find.hpp"
#include "sim/llvm_namespace.h"
#include "logging/logger.h"
#include <sstream>


namespace sim {

  class Llvm_constexpr_scanner
      : public ast::Scanner_base<Llvm_constexpr_scanner> {
    public:
      Llvm_constexpr_scanner(std::shared_ptr<Llvm_constant> constant,
          Llvm_namespace& ns);

      std::shared_ptr<Llvm_constant> constant_of_node(ast::Node_if const& node) {
        auto rv = std::make_shared<Llvm_constant>();
        rv->impl.expr = m_values.at(&node);
        rv->type = m_types.at(&node);
        return rv;
      }

    private:
      typedef std::unordered_map<ast::Node_if const*,
          llvm::Constant*> Node_value_map;
      typedef std::unordered_map<ast::Node_if const*, std::shared_ptr<Llvm_type>> Node_type_map;

      log4cxx::LoggerPtr m_logger;
      std::shared_ptr<Llvm_constant> m_constant;
      Llvm_namespace& m_ns;
      Node_value_map m_values;
      Node_type_map m_types;

      virtual bool visit_literal_int(ast::Literal<int> const& node);
      virtual bool visit_literal_double(ast::Literal<double> const& node);
      virtual bool visit_literal_bool(ast::Literal<bool> const& node);
      virtual bool visit_literal_string(ast::Literal<std::string> const& node);
      virtual bool leave_constant_def(ast::Constant_def const& node);
      virtual bool leave_table_def_item(ast::Table_def_item const& node);
      virtual bool enter_constant_ref(ast::Constant_ref const& node);
      virtual bool leave_op_not(ast::Op_not const& node);
      virtual bool leave_op_equal(ast::Op_equal const& node);
      virtual bool leave_op_plus(ast::Op_plus const& node);
      virtual bool leave_op_minus(ast::Op_minus const& node);
      virtual bool leave_op_mult(ast::Op_mult const& node);
      virtual bool leave_op_div(ast::Op_div const& node);
      virtual bool leave_op_mod(ast::Op_mod const& node);
      virtual bool leave_op_and(ast::Op_and const& node);
      virtual bool leave_op_or(ast::Op_or const& node);
      virtual bool leave_op_gt(ast::Op_greater_then const& node);
      virtual bool leave_op_lt(ast::Op_lesser_then const& node);
      virtual bool leave_op_ge(ast::Op_greater_or_equal_then const& node);
      virtual bool leave_op_le(ast::Op_lesser_or_equal_then const& node);


      template<typename Node>
      bool insert_bin_op(Node const& node, std::string const& opname) {
        // get types and values
        auto ty_left = m_types.at(&(node.left()));
        auto ty_right = m_types.at(&(node.right()));
        auto v_left = m_values.at(&(node.left()));
        auto v_right = m_values.at(&(node.right()));

        // select an operator
        std::shared_ptr<Llvm_operator> op = ir::find_operator(m_ns,
            opname,
            ty_left,
            ty_right);
        if( op && op->impl.const_insert_func ) {
          auto v_cmp = op->impl.const_insert_func(v_left, v_right);
          m_values[&node] = v_cmp;
          m_types[&node] = op->return_type;
        } else {
          std::stringstream strm;
          strm << node.location() << ": failed to find constant operator '"
            << opname
            << "' with signature: ["
            << ty_left->name
            << "] " << opname << " ["
            << ty_right->name
            << "]";
          throw std::runtime_error(strm.str());
        }

        return true;
      }
  };

}


/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
