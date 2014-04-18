#include "ir/codegen.h"

namespace ir {

  Codegen_base::Codegen_base() {
  }

  Codegen_base::~Codegen_base() {
  }

  Null_codegen::Null_codegen()
    : Codegen_base() {
  }

  std::shared_ptr<Codeblock_if>
  Null_codegen::make_codeblock() {
    return std::shared_ptr<Codeblock_if>(new Null_codeblock());
  }

}
