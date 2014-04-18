#pragma once

#include "ir/namespace.h"
#include "ast/visitor.h"
#include "sim/llvm_codeblock.h"

namespace sim {

  class Codegen_visitor : public ast::Visitor_base {
    public:
      Codegen_visitor(ir::Namespace const& ns, Llvm_codeblock& block);

      virtual bool visit(ast::Node_if const& node);

    private:
      ir::Namespace const& m_ns;
      Llvm_codeblock& m_codeblock;
  };

}
