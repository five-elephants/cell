#include "llvm_function_scanner.h"

#include "ir/find.hpp"
#include "ir/find_hierarchy.h"


namespace sim {

  Llvm_function_scanner::Llvm_function_scanner(Llvm_namespace& ns, Llvm_function& function)
    : m_ns(ns),
      m_function(function) {
    using namespace llvm;

    auto lib = ir::find_library(ns);
    auto name = ir::hierarchical_name(ns, function.name);

    std::cout << "creating function '" << name << "'" << std::endl;

    // create function type
    function.impl.func_type = get_function_type(function);

    // create function itself
    function.impl.code = Function::Create(function.impl.func_type,
        Function::ExternalLinkage,
        name,
        lib->impl.module.get());
  }


  Llvm_function_scanner::Llvm_function_scanner(Llvm_module& mod, Llvm_function& function)
    : m_ns(mod),
      m_mod(&mod),
      m_function(function) {
    using namespace llvm;

    auto lib = ir::find_library(m_ns);
    auto name = ir::hierarchical_name(m_ns, function.name);

    std::cout << "creating function within module '" << name << "'" << std::endl;

    // create function type
    function.impl.func_type = get_function_type(function);

    // create function itself
    function.impl.code = Function::Create(function.impl.func_type,
        Function::ExternalLinkage,
        name,
        lib->impl.module.get());
  }


  llvm::FunctionType*
  Llvm_function_scanner::get_function_type(Llvm_function const& function) const {
    using namespace llvm;

    auto lib = ir::find_library(m_ns);
    std::vector<Type*> args;

    if( m_mod ) {
      auto r = PointerType::getUnqual(m_mod->impl.mod_type);
      args.push_back(r);
      args.push_back(r);
      args.push_back(PointerType::getUnqual(read_mask_type()));
    }

    for(auto p : function.parameters) {
      args.push_back(p->type->impl.type);
    }

    return FunctionType::get(Type::getVoidTy(lib->impl.context),
        args,
        false);
  }


  llvm::ArrayType*
  Llvm_function_scanner::read_mask_type() const { 
    if( m_mod ) {
      auto lib = ir::find_library(m_ns);
      auto mod_type = m_mod->impl.mod_type;
      return llvm::ArrayType::get(llvm::IntegerType::get(lib->impl.context, 1),
          mod_type->getNumElements());
    } else {
      throw std::runtime_error("read_mask makes only sense for functions within modules, but m_mod == nullptr");
    }

    return nullptr;
  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
