#pragma once

#include "ir/codeblock.h"

namespace sim {

  class Llvm_codeblock : public ir::Codeblock_base {
    public:
      virtual void scan_ast(ir::Namespace& enclosing_ns,
          ast::Node_if const& tree);
  };

}
