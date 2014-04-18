#pragma once

#include "ir/codegen.h"

namespace sim {

  class Llvm_codegen : public ir::Codegen_base {
    public:
      Llvm_codegen();

      virtual std::shared_ptr<ir::Codeblock_if> make_codeblock();

    private:

  };

}
