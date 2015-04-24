#include "llvm_constexpr_scanner.h"

#include "ir/builtins.h"


namespace sim {

  Llvm_constexpr_scanner::Llvm_constexpr_scanner(std::shared_ptr<Llvm_constant> c)
      : m_constant(c) {
    this->template on_visit_if_type<ast::Literal<int>>(
        &Llvm_constexpr_scanner::visit_literal_int);
    this->template on_leave_if_type<ast::Constant_def>(
        &Llvm_constexpr_scanner::leave_constant_def);
  }


  bool
  Llvm_constexpr_scanner::visit_literal_int(ast::Literal<int> const& node) {
    using namespace llvm;

    m_values[&node] = ConstantInt::get(getGlobalContext(),
        APInt(64, node.value(), true));

    return true;
  }


  bool
  Llvm_constexpr_scanner::leave_constant_def(ast::Constant_def const& node) {
    m_constant->impl.expr = m_values.at(&node.expression());
    return true;
  }

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
