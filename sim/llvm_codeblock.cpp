#include "sim/llvm_codeblock.h"

namespace sim {

  Llvm_codeblock::Llvm_codeblock(llvm::LLVMContext& context,
        llvm::IRBuilder<>& builder,
        std::shared_ptr<llvm::Module> module)
    : Codeblock_base(),
      m_context(context),
      m_builder(builder),
      m_module(module) {
  }

  void
  Llvm_codeblock::scan_ast(ir::Namespace& enclosing_ns,
      ast::Node_if const& tree) {
  }

}