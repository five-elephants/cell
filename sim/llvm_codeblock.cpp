#include "sim/llvm_codeblock.h"

#include "sim/codegen_visitor.h"

namespace sim {

  using namespace llvm;

  Llvm_codeblock::Llvm_codeblock(Llvm_codegen const& parent,
        llvm::LLVMContext& context,
        llvm::IRBuilder<>& builder,
        std::shared_ptr<llvm::Module> module,
        ir::Namespace const& ns)
    : Codeblock_base(),
      m_codegen(parent),
      m_context(context),
      m_builder(builder),
      m_module(module),
      m_enclosing_ns(ns) {
    // create anonymous function and set builder insertion point
    m_function_type = FunctionType::get(Type::getVoidTy(m_context), false);
    m_function = Function::Create(m_function_type,
        Function::ExternalLinkage,
        "",
        m_module.get());
    m_bb = BasicBlock::Create(m_context, "entry", m_function);
    m_builder.SetInsertPoint(m_bb);
  }


  void
  Llvm_codeblock::scan_ast(ast::Node_if const& tree) {
    Codegen_visitor visitor(m_enclosing_ns, *this);
    tree.accept(visitor);
  }


  void
  Llvm_codeblock::append_predefined_objects(
      std::map<ir::Label, std::shared_ptr<ir::Object>> m) {
    m_predefined_objects.insert(m.begin(), m.end());
  }



}
