#include "llvm_codegen.h"

#include "llvm_codeblock.h"
#include "ir/builtins.h"

#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <memory>
#include <iostream>

namespace sim {

  Llvm_codegen::Llvm_codegen() 
    : ir::Codegen_base(),
      m_context(llvm::getGlobalContext()),
      m_builder(llvm::getGlobalContext()),
      m_module(new llvm::Module("top", llvm::getGlobalContext())),
      m_fpm(m_module.get()) {
    m_fpm.add(llvm::createBasicAliasAnalysisPass());
    m_fpm.add(llvm::createPromoteMemoryToRegisterPass());
    m_fpm.add(llvm::createInstructionCombiningPass());
    m_fpm.add(llvm::createReassociatePass());
    m_fpm.add(llvm::createGVNPass());
    m_fpm.add(llvm::createCFGSimplificationPass());
    m_fpm.doInitialization();
  }


  std::shared_ptr<ir::Codeblock_if>
  Llvm_codegen::make_codeblock(ir::Namespace const& ns) {
    auto rv = std::make_shared<Llvm_codeblock>(*this,
          m_context,
          m_builder,
          m_module,
          ns);
    return rv;
  }


  void
  Llvm_codegen::register_variable(std::shared_ptr<ir::Object> obj) {
    auto gv = new llvm::GlobalVariable(*m_module,
        get_type(obj->type),
        false,
        llvm::GlobalVariable::InternalLinkage,
        //llvm::GlobalVariable::ExternalLinkage,
        llvm::ConstantInt::get(m_context, llvm::APInt(64, 0, true)),
        obj->name);
    if( !gv )
      throw std::runtime_error("failed to register global variable");

    m_globals[obj] = gv;
  }


  void
  Llvm_codegen::emit() {
    std::cout << "\n ==== Code: ====\n" << std::endl;
    m_module->dump();
    std::cout << "\n ====  END  ====\n" << std::endl;
  }


  llvm::Type*
  Llvm_codegen::get_type(std::shared_ptr<ir::Type> type) const {
    if( type == ir::Builtins::types["int"] )
      return llvm::Type::getInt64Ty(m_context);
    else if( type == ir::Builtins::types["void"] )
      return llvm::Type::getVoidTy(m_context);

    return nullptr;
  }


  llvm::Value*
  Llvm_codegen::get_global(std::shared_ptr<ir::Object> object) const {
    auto it = m_globals.find(object);
    if( it != m_globals.end() )
      return it->second;

    return nullptr;
  }


  void
  Llvm_codegen::optimize(llvm::Function* func) {
    m_fpm.run(*func);
  }

}

