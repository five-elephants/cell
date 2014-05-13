#include "llvm_codegen.h"

#include "llvm_codeblock.h"
#include "ir/builtins.h"

#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/TypeBuilder.h>
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

    define_builtin_types();
    declare_runtime_functions();
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
  Llvm_codegen::register_global(std::shared_ptr<ir::Object> obj) {
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
  Llvm_codegen::register_function(std::shared_ptr<ir::Function> func,
      llvm::Function* code) {
    m_functions[func.get()] = code;
  }


  void
  Llvm_codegen::register_process(std::shared_ptr<ir::Process> func,
      llvm::Function* code) {
    m_processes[func.get()] = code;
  }


  void
  Llvm_codegen::register_module_ctor(ir::Module* mod, llvm::Function* func) {
    m_module_ctors[mod] = func;
  }

  void
  Llvm_codegen::register_module_init(ir::Module* mod, llvm::Function* func) {
    m_module_inits[mod] = func;
  }


  void
  Llvm_codegen::emit() {
    std::cout << "\n ==== Code: ====\n" << std::endl;
    m_module->dump();
    std::cout << "\n ====  END  ====\n" << std::endl;
  }


  void
  Llvm_codegen::create_setup(std::shared_ptr<ir::Module> toplevel) {
    using namespace llvm;

    auto toplevel_type = get_module_type(toplevel.get());
    auto ctor = get_module_ctor(toplevel.get());
    auto undef_init = UndefValue::get(toplevel_type);
    m_root = new llvm::GlobalVariable(*m_module,
        toplevel_type,
        false,
        llvm::GlobalVariable::InternalLinkage,
        //llvm::GlobalVariable::ExternalLinkage,
        undef_init,
        "root");
    //if( !m_root )
      //throw std::runtime_error("failed to register root hierarchy module");

    auto setup_func = Function::Create(FunctionType::get(Type::getVoidTy(m_context), false),
      Function::ExternalLinkage,
      "setup",
      m_module.get());

    auto bb = BasicBlock::Create(m_context, "entry", setup_func);
    m_builder.SetInsertPoint(bb);

    auto root_init = m_builder.CreateCall(ctor);
    m_builder.CreateStore(root_init, m_root);
    m_builder.CreateRetVoid();
  }


  llvm::Type*
  Llvm_codegen::get_type(std::shared_ptr<ir::Type> type) const {
    auto it = m_type_map.find(type.get());
    if( it != m_type_map.end() )
      return it->second;

    return nullptr;
  }


  llvm::Value*
  Llvm_codegen::get_global(std::shared_ptr<ir::Object> object) const {
    auto it = m_globals.find(object);
    if( it != m_globals.end() )
      return it->second;

    return nullptr;
  }


  llvm::Function*
  Llvm_codegen::get_function(std::shared_ptr<ir::Function> func) const {
    auto it = m_functions.find(func.get());
    if( it != m_functions.end() )
      return it->second;

    return nullptr;
  }


  llvm::Function*
  Llvm_codegen::get_process(std::shared_ptr<ir::Process> func) const {
    auto it = m_processes.find(func.get());
    if( it != m_processes.end() )
      return it->second;

    return nullptr;
  }




  void
  Llvm_codegen::optimize(llvm::Function* func) {
    m_fpm.run(*func);
  }


  void
  Llvm_codegen::define_builtin_types() {
    using namespace ir;

    m_type_map[Builtins::types.at("int").get()] = llvm::Type::getInt64Ty(m_context);
    m_type_map[Builtins::types.at("void").get()] = llvm::Type::getVoidTy(m_context);
    m_type_map[Builtins::types.at("string").get()] = llvm::TypeBuilder<char*, false>::get(m_context);
  }


  void
  Llvm_codegen::declare_runtime_functions() {
    using namespace llvm;

    for(auto func_it : ir::Builtins::functions) {
      std::vector<Type*> arg_types;
      auto func = func_it.second;

      for(auto p : func->parameters) {
        arg_types.push_back(get_type(p->type));
      }

      FunctionType* type = FunctionType::get(get_type(func->return_type),
        arg_types,
        false);

      Function* f = Function::Create(type,
          Function::ExternalLinkage,
          func->name,
          m_module.get());

      register_function(func, f);
    }
  }


}

