#include "sim/llvm_codeblock.h"

#include "sim/codegen_visitor.h"

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
    Codegen_visitor visitor(enclosing_ns, *this);

    tree.accept(visitor);
  }

  void
  Llvm_codeblock::append_predefined_objects(
      std::map<ir::Label, std::shared_ptr<ir::Object>> m) {
    m_predefined_objects.insert(m.begin(), m.end());
  }

}
