#include "llvm_codegen.h"

#include "llvm_codeblock.h"

namespace sim {

  Llvm_codegen::Llvm_codegen() 
    : ir::Codegen_base() {
  }

  std::shared_ptr<ir::Codeblock_if>
  Llvm_codegen::make_codeblock() {
    return std::shared_ptr<ir::Codeblock_if>(new Llvm_codeblock());  
  }

}

