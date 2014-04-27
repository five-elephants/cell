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
  }


  void
  Llvm_codeblock::scan_ast(ast::Node_if const& tree) {
    Codegen_visitor visitor(m_enclosing_ns, *this);

    m_function = Function::Create(m_function_type,
        Function::ExternalLinkage,
        m_function_name,
        m_module.get());

    if( m_prototype ) {
      // this is a function declaration, so add argument names 
      auto i = m_function->arg_begin();
      auto j = m_prototype->parameters.begin();
      for( ; 
          (i != m_function->arg_end()) && (j != m_prototype->parameters.end()); 
          ++i, ++j) {
        i->setName(j->first);
        visitor.add_named_value(j->first, i);
      }
    }

    m_bb = BasicBlock::Create(m_context, "entry", m_function);
    m_builder.SetInsertPoint(m_bb);

    tree.accept(visitor);
  }


  void
  Llvm_codeblock::append_predefined_objects(
      std::map<ir::Label, std::shared_ptr<ir::Object>> m) {
    m_predefined_objects.insert(m.begin(), m.end());
  }


  void
  Llvm_codeblock::prototype(std::shared_ptr<ir::Function> func) {
    m_prototype = func;

    std::vector<Type*> arg_types;
    for(auto p : func->parameters) {
      arg_types.push_back(get_type(p.second->type->name));
    }
    m_function_type = FunctionType::get(m_codegen.get_type(func->return_type),
        arg_types,
        false);
    m_function_name = func->name;
  }

}
