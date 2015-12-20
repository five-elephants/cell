#include "llvm_constexpr_scanner.h"

#include "ir/builtins.h"
#include "ir/find.hpp"
#include "ir/find_hierarchy.h"
#include <sstream>
#include <stdexcept>
#include <boost/algorithm/string.hpp>


namespace sim {

  Llvm_constexpr_scanner::Llvm_constexpr_scanner(std::shared_ptr<Llvm_constant> c,
      Llvm_namespace& ns)
      : m_logger(log4cxx::Logger::getLogger("cell.scan")),
        m_constant(c),
        m_ns(ns) {
    on_visit_if_type<ast::Literal<int>>(
        &Llvm_constexpr_scanner::visit_literal_int);
    on_visit_if_type<ast::Literal<double>>(
        &Llvm_constexpr_scanner::visit_literal_double);
    on_visit_if_type<ast::Literal<bool>>(
        &Llvm_constexpr_scanner::visit_literal_bool);
    on_visit_if_type<ast::Literal<std::string>>(
        &Llvm_constexpr_scanner::visit_literal_string);
    on_leave_if_type<ast::Constant_def>(
        &Llvm_constexpr_scanner::leave_constant_def);
    on_leave_if_type<ast::Table_def_item>(
        &Llvm_constexpr_scanner::leave_table_def_item);
    on_enter_if_type<ast::Constant_ref>(
        &Llvm_constexpr_scanner::enter_constant_ref);

    on_leave_if_type<ast::Op_equal>(
        &Llvm_constexpr_scanner::leave_op_equal);
    on_leave_if_type<ast::Op_not>(
        &Llvm_constexpr_scanner::leave_op_not);
    on_leave_if_type<ast::Op_plus>(
        &Llvm_constexpr_scanner::leave_op_plus);
    on_leave_if_type<ast::Op_minus>(
        &Llvm_constexpr_scanner::leave_op_minus);
    on_leave_if_type<ast::Op_mult>(
        &Llvm_constexpr_scanner::leave_op_mult);
    on_leave_if_type<ast::Op_div>(
        &Llvm_constexpr_scanner::leave_op_div);
    on_leave_if_type<ast::Op_mod>(
        &Llvm_constexpr_scanner::leave_op_mod);
    on_leave_if_type<ast::Op_and>(
        &Llvm_constexpr_scanner::leave_op_and);
    on_leave_if_type<ast::Op_or>(
        &Llvm_constexpr_scanner::leave_op_or);
    on_leave_if_type<ast::Op_greater_then>(
        &Llvm_constexpr_scanner::leave_op_gt);
    on_leave_if_type<ast::Op_lesser_then>(
        &Llvm_constexpr_scanner::leave_op_lt);
    on_leave_if_type<ast::Op_greater_or_equal_then>(
        &Llvm_constexpr_scanner::leave_op_ge);
    on_leave_if_type<ast::Op_lesser_or_equal_then>(
        &Llvm_constexpr_scanner::leave_op_le);
  }


  bool
  Llvm_constexpr_scanner::visit_literal_int(ast::Literal<int> const& node) {
    using namespace llvm;

    m_values[&node] = ConstantInt::get(getGlobalContext(),
        APInt(64, node.value(), true));
    m_types[&node] = ir::Builtins<Llvm_impl>::types.at("int");

    return true;
  }


  bool
  Llvm_constexpr_scanner::visit_literal_double(ast::Literal<double> const& node) {
    using namespace llvm;

    auto ty = ir::Builtins<Llvm_impl>::types.at("float");
    auto v = ConstantFP::get(ty->impl.type, node.value());
    m_values[&node] = v;
    m_types[&node] = ty;

    return true;
  }


  bool
  Llvm_constexpr_scanner::visit_literal_bool(ast::Literal<bool> const& node) {
    using namespace llvm;

    auto v = ConstantInt::get(getGlobalContext(),
        APInt(1, node.value(), true));
    m_values[&node] = v;
    m_types[&node] = ir::Builtins<Llvm_impl>::types.at("bool");

    return true;
  }


  bool
  Llvm_constexpr_scanner::visit_literal_string(ast::Literal<std::string> const& node) {
    using namespace llvm;

    //auto v = m_builder.CreateGlobalStringPtr(node.value(), "stringconst");
    auto v = ConstantDataArray::getString(getGlobalContext(),
        node.value());
    m_values[&node] = v;
    m_types[&node] = ir::Builtins<Llvm_impl>::types.at("string");

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_constant_def(ast::Constant_def const& node) {
    m_constant->impl.expr = m_values.at(&node.expression());

    if( m_constant->type ) {
      // check type
      if( m_constant->type != m_types.at(&node.expression()) ) {
        std::stringstream strm;
        strm << node.location() << ": "
          << "type mismatch in constant definition: "
          << "Declared type is '"
          << m_constant->type->name
          << "', inferred type is '"
          << m_types[&node.expression()]->name
          << "'";
        throw std::runtime_error(strm.str());
      }
    } else {
      // use inferred type
      m_constant->type = m_types.at(&node.expression());
    }

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_table_def_item(ast::Table_def_item const& node) {
    m_constant->impl.expr = m_values.at(node.value());

    if( m_constant->type ) {
      // check type
      if( m_constant->type->array_base_type != m_types.at(node.value()) ) {
        std::stringstream strm;
        strm << node.location() << ": "
          << "type mismatch in constant definition: "
          << "Declared type is '"
          << m_constant->type->name
          << "', inferred type is '"
          << m_types[node.value()]->name
          << "'";
        throw std::runtime_error(strm.str());
      }
    } else {
      // use inferred type
      m_constant->type = m_types.at(node.value());
    }

    LOG4CXX_TRACE(m_logger, "Leaving Table_def_item (impl.expr = "
        << m_constant->impl.expr);

    return true;
  }


  bool
  Llvm_constexpr_scanner::enter_constant_ref(ast::Constant_ref const& node) {
    std::shared_ptr<Llvm_constant> p;

    auto const& qn = node.name();
    if( qn.size() > 1 ) {
      p = ir::find_by_path(m_ns,
          &Llvm_namespace::constants,
          qn);
    } else {
      p = ir::find_constant(m_ns, qn[0]);
    }

    if( !p ) {
      std::stringstream strm;
      strm << node.location() << ": No constant with name '"
        << boost::algorithm::join(qn, "::")
        << "' was previously declared and is visible.";
      throw std::runtime_error(strm.str());
    }

    m_values[&node] = p->impl.expr;
    m_types[&node] = p->type;

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_not(ast::Op_not const& node) {
    auto ty = m_types.at(&(node.operand()));
    auto value = m_values.at(&(node.operand()));

    // select an operator
    std::shared_ptr<Llvm_operator> op = ir::find_operator(m_ns,
        "!",
        ty,
        ty);

    if( op && op->impl.const_insert_func ) {
      auto v = op->impl.const_insert_func(value, value);
      m_values[&node] = v;
      m_types[&node] = op->return_type;
    } else {
      std::stringstream strm;
      strm << node.location() << ": failed to find constant operator '"
        << "!"
        << "' with signature: ["
        << ty->name
        << "]";
      throw std::runtime_error(strm.str());
    }

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_equal(ast::Op_equal const& node) {
    insert_bin_op(node, "==");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_plus(ast::Op_plus const& node) {
    insert_bin_op(node, "+");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_minus(ast::Op_minus const& node) {
    insert_bin_op(node, "-");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_mult(ast::Op_mult const& node) {
    insert_bin_op(node, "*");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_div(ast::Op_div const& node) {
    insert_bin_op(node, "/");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_mod(ast::Op_mod const& node) {
    insert_bin_op(node, "%");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_and(ast::Op_and const& node) {
    insert_bin_op(node, "&&");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_or(ast::Op_or const& node) {
    insert_bin_op(node, "||");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_gt(ast::Op_greater_then const& node) {
    insert_bin_op(node, ">");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_lt(ast::Op_lesser_then const& node) {
    insert_bin_op(node, "<");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_ge(ast::Op_greater_or_equal_then const& node) {
    insert_bin_op(node, ">=");
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_le(ast::Op_lesser_or_equal_then const& node) {
    insert_bin_op(node, "<=");
    return true;
  }

}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
