#include "llvm_codegen.h"

#include "llvm_codeblock.h"

#include <memory>

namespace sim {

  Llvm_codegen::Llvm_codegen() 
    : ir::Codegen_base(),
      m_context(llvm::getGlobalContext()),
      m_builder(llvm::getGlobalContext()),
      m_module(new llvm::Module("top", llvm::getGlobalContext())) {
  }

  std::shared_ptr<ir::Codeblock_if>
  Llvm_codegen::make_codeblock() {
    auto rv = std::make_shared<Llvm_codeblock>(m_context,
          m_builder,
          m_module);
    return rv;
  }

}

