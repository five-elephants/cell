#include "llvm_constexpr_scanner.h"

#include "ir/builtins.h"
#include "ir/find.hpp"
#include <sstream>
#include <stdexcept>


namespace sim {

  Llvm_constexpr_scanner::Llvm_constexpr_scanner(std::shared_ptr<Llvm_constant> c,
      Llvm_namespace& ns)
      : m_constant(c),
        m_ns(ns) {
    this->template on_visit_if_type<ast::Literal<int>>(
        &Llvm_constexpr_scanner::visit_literal_int);
    this->template on_visit_if_type<ast::Literal<double>>(
        &Llvm_constexpr_scanner::visit_literal_double);
    this->template on_visit_if_type<ast::Literal<bool>>(
        &Llvm_constexpr_scanner::visit_literal_bool);
    this->template on_visit_if_type<ast::Literal<std::string>>(
        &Llvm_constexpr_scanner::visit_literal_string);
    this->template on_leave_if_type<ast::Constant_def>(
        &Llvm_constexpr_scanner::leave_constant_def);
    this->template on_enter_if_type<ast::Constant_ref>(
        &Llvm_constexpr_scanner::enter_constant_ref);

    this->template on_leave_if_type<ast::Op_not>(
        &Llvm_constexpr_scanner::leave_op_not);
    this->template on_leave_if_type<ast::Op_equal>(
        &Llvm_constexpr_scanner::leave_op_equal);
    this->template on_leave_if_type<ast::Op_plus>(
        &Llvm_constexpr_scanner::leave_op_plus);
    this->template on_leave_if_type<ast::Op_minus>(
        &Llvm_constexpr_scanner::leave_op_minus);
    this->template on_leave_if_type<ast::Op_mult>(
        &Llvm_constexpr_scanner::leave_op_mult);
    this->template on_leave_if_type<ast::Op_div>(
        &Llvm_constexpr_scanner::leave_op_div);
    this->template on_leave_if_type<ast::Op_mod>(
        &Llvm_constexpr_scanner::leave_op_mod);
    this->template on_leave_if_type<ast::Op_and>(
        &Llvm_constexpr_scanner::leave_op_and);
    this->template on_leave_if_type<ast::Op_or>(
        &Llvm_constexpr_scanner::leave_op_or);
    this->template on_leave_if_type<ast::Op_greater_then>(
        &Llvm_constexpr_scanner::leave_op_gt);
    this->template on_leave_if_type<ast::Op_lesser_then>(
        &Llvm_constexpr_scanner::leave_op_lt);
    this->template on_leave_if_type<ast::Op_greater_or_equal_then>(
        &Llvm_constexpr_scanner::leave_op_ge);
    this->template on_leave_if_type<ast::Op_lesser_or_equal_then>(
        &Llvm_constexpr_scanner::leave_op_le);
  }


  bool
  Llvm_constexpr_scanner::visit_literal_int(ast::Literal<int> const& node) {
    using namespace llvm;

    m_values[&node] = ConstantInt::get(getGlobalContext(),
        APInt(64, node.value(), true));

    return true;
  }


  bool
  Llvm_constexpr_scanner::visit_literal_double(ast::Literal<double> const& node) {
    using namespace llvm;

    auto ty = ir::Builtins<Llvm_impl>::types.at("float");
    auto v = ConstantFP::get(ty->impl.type, node.value());
    m_values[&node] = v;

    return true;
  }


  bool
  Llvm_constexpr_scanner::visit_literal_bool(ast::Literal<bool> const& node) {
    using namespace llvm;

    auto v = ConstantInt::get(getGlobalContext(),
        APInt(1, node.value(), true));
    m_values[&node] = v;

    return true;
  }


  bool
  Llvm_constexpr_scanner::visit_literal_string(ast::Literal<std::string> const& node) {
    using namespace llvm;

    //auto v = m_builder.CreateGlobalStringPtr(node.value(), "stringconst");
    auto v = ConstantDataArray::getString(getGlobalContext(),
        node.value());
    m_values[&node] = v;

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_constant_def(ast::Constant_def const& node) {
    m_constant->impl.expr = m_values.at(&node.expression());
    return true;
  }


  bool
  Llvm_constexpr_scanner::enter_constant_ref(ast::Constant_ref const& node) {
    auto p = ir::find_constant(m_ns, node.identifier().identifier());
    if( !p ) {
      std::stringstream strm;
      strm << node.location() << ": No constant with name '"
        << node.identifier().identifier()
        << "' was previously declared and is visible.";
      throw std::runtime_error(strm.str());
    }

    m_values[&node] = p->impl.expr;

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_not(ast::Op_not const& node) {
    auto v_oper = m_values.at(&(node.operand()));
    m_values[&node] = llvm::ConstantExpr::getNot(v_oper);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_equal(ast::Op_equal const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    // XXX this should check the type...
    m_values[&node] = llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_EQ,
        v_left,
        v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_plus(ast::Op_plus const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getAdd(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_minus(ast::Op_minus const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getSub(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_mult(ast::Op_mult const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getMul(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_div(ast::Op_div const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getSDiv(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_mod(ast::Op_mod const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getSRem(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_and(ast::Op_and const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getAnd(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_or(ast::Op_or const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getOr(v_left, v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_gt(ast::Op_greater_then const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_SGT,
        v_left,
        v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_lt(ast::Op_lesser_then const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_SLT,
        v_left,
        v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_ge(ast::Op_greater_or_equal_then const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_SGE,
        v_left,
        v_right);
    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_op_le(ast::Op_lesser_or_equal_then const& node) {
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));
    m_values[&node] = llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_SLE,
        v_left,
        v_right);
    return true;
  }

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
