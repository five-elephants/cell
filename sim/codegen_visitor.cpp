#include "sim/codegen_visitor.h"

namespace sim {

  Codegen_visitor::Codegen_visitor(ir::Namespace const& ns, Llvm_codeblock& block)
    : Visitor_base(),
      m_ns(ns),
      m_codeblock(block) {
  }


  bool
  Codegen_visitor::visit(ast::Node_if const& node) {
    return true;
  }

}
