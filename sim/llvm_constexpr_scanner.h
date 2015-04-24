#pragma once

#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "ir/namespace.h"
#include "sim/llvm_namespace.h"


namespace sim {

  class Llvm_constexpr_scanner
      : public ast::Scanner_base<Llvm_constexpr_scanner> {
    public:
      Llvm_constexpr_scanner(std::shared_ptr<Llvm_constant>);


    private:
      typedef std::unordered_map<ast::Node_if const*,
          llvm::Constant*> Node_value_map;

      std::shared_ptr<Llvm_constant> m_constant;
      Node_value_map m_values;

      virtual bool visit_literal_int(ast::Literal<int> const& node);
      virtual bool leave_constant_def(ast::Constant_def const& node);
  };

}


/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
