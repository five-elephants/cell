#include "llvm_namespace.h"


namespace sim {

  Llvm_impl::Library
  create_library_impl(std::string const& name) {
    Llvm_impl::Library rv;

    rv.builder.reset(new llvm::IRBuilder<>(rv.context));
    rv.module.reset(new llvm::Module("library", rv.context));
    rv.fpm.reset(new llvm::FunctionPassManager(rv.module.get()));

    rv.fpm->add(llvm::createBasicAliasAnalysisPass());
    rv.fpm->add(llvm::createPromoteMemoryToRegisterPass());
    rv.fpm->add(llvm::createInstructionCombiningPass());
    rv.fpm->add(llvm::createReassociatePass());
    rv.fpm->add(llvm::createGVNPass());
    rv.fpm->add(llvm::createCFGSimplificationPass());
    rv.fpm->doInitialization();

    return rv;
  }

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
