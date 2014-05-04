#include "sim/llvm_codeblock.h"

#include "sim/codegen_visitor.h"
#include "sim/module_codegen_visitor.h"

#include <llvm/Analysis/Verifier.h>
#include <sstream>

namespace sim {

  using namespace llvm;

  Llvm_codeblock::Llvm_codeblock(Llvm_codegen& parent,
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

    m_bb = BasicBlock::Create(m_context, "entry", m_function);
    m_builder.SetInsertPoint(m_bb);

    // make local mutable copies of the arguments
    if( m_prototype ) {
      auto i = m_function->arg_begin();
      if( m_enclosing_mod ) {
        i->setName("this");
        ++i;
      }
      auto j = m_prototype->parameters.begin();
      for( ; 
          (i != m_function->arg_end()) && (j != m_prototype->parameters.end()); 
          ++i, ++j) {
        i->setName((*j)->name);

        auto ptr = m_builder.CreateAlloca(i->getType(), 0, (*j)->name);
        auto v = m_builder.CreateStore(i, ptr);

        visitor.add_named_value((*j)->name, ptr);
      }
    }

    tree.accept(visitor);

    if( !m_function->back().getTerminator() ) {
      // TODO not sure if this is enough
      m_builder.CreateRetVoid();
    }

    if( verifyFunction(*m_function, PrintMessageAction) ) {
      std::stringstream strm;
      strm << tree.location()
        << ": during generation of function '"
        << m_function_name << "': "
        << "error during code generation. (verifyFunction returned false)";
      throw std::runtime_error(strm.str());
    }

    m_codegen.optimize(m_function);
  }


  void
  Llvm_codeblock::scan_ast_module(ast::Node_if const& tree) {
    Module_codegen_visitor visitor(*this);
    tree.accept(visitor);

    m_function_name = std::string("ctor_") + m_enclosing_mod->name;
    
    auto mod_type = visitor.get_module_type(m_enclosing_mod->name);
    m_codegen.add_module_type(m_enclosing_mod, mod_type);

    m_function_type = FunctionType::get(mod_type, false);

    m_function = Function::Create(m_function_type,
        Function::ExternalLinkage,
        m_function_name,
        m_module.get());

    m_bb = BasicBlock::Create(m_context, "entry", m_function);
    m_builder.SetInsertPoint(m_bb);

    // create intialization method
    auto obj_ptr = m_builder.CreateAlloca(mod_type, nullptr, "rv");
    auto rv = visitor.get_initialization(obj_ptr);

    // TODO call __init__ if defined

    m_builder.CreateRet(rv);

    if( verifyFunction(*m_function, PrintMessageAction) ) {
      std::stringstream strm;
      strm << tree.location()
        << ": during generation of constructor '"
        << m_function_name << "': "
        << "error during code generation. (verifyFunction returned false)";
      throw std::runtime_error(strm.str());
    }

    m_codegen.optimize(m_function);
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
    if( m_enclosing_mod )
      arg_types.push_back(PointerType::getUnqual(m_codegen.get_module_type(m_enclosing_mod)));

    for(auto p : func->parameters) {
      arg_types.push_back(get_type(p->type->name));
    }
    m_function_type = FunctionType::get(m_codegen.get_type(func->return_type),
        arg_types,
        false);
    m_function_name = func->name;
  }

  void
  Llvm_codeblock::enclosing_module(ir::Module* mod) {
    m_enclosing_mod = mod;
  }



  llvm::AllocaInst*
  Llvm_codeblock::allocate_variable(ir::Label const& name, ir::Label const& type_name) const {
    auto type = get_type(type_name);
    if( type ) {
      IRBuilder<> bld(&m_function->getEntryBlock(), m_function->getEntryBlock().begin());
      return bld.CreateAlloca(type, 0, name.c_str());
    }

    return nullptr;
  }
}
