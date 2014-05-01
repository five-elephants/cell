#include "llvm_codegen.h"

#include "llvm_codeblock.h"
#include "ir/builtins.h"

#include <memory>

namespace sim {

  Llvm_codegen::Llvm_codegen() 
    : ir::Codegen_base(),
      m_context(llvm::getGlobalContext()),
      m_builder(llvm::getGlobalContext()),
      m_module(new llvm::Module("top", llvm::getGlobalContext())) {
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
    auto gv = new llvm::GlobalVariable( get_type(obj->type),
        false,
        llvm::GlobalVariable::InternalLinkage,
        nullptr,
        obj->name);
    if( !gv )
      throw std::runtime_error("failed to register global variable");

    m_globals[obj] = gv;
  }


  void
  Llvm_codegen::emit() {
    m_module->dump();
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

}

