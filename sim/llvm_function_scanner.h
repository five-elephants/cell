#pragma once 

#include "ast/scanner_base.h"

#include "llvm_namespace.h"


namespace sim {

  class Llvm_function_scanner : public ast::Scanner_base<Llvm_function_scanner> {
    public:
      Llvm_function_scanner(Llvm_namespace& ns, Llvm_function& function);
      Llvm_function_scanner(Llvm_module& ns, Llvm_function& function);


    private:
      Llvm_namespace& m_ns;
      Llvm_module* m_mod = nullptr; 
      Llvm_function& m_function;

      llvm::FunctionType* get_function_type(Llvm_function const& function) const;
      llvm::ArrayType* read_mask_type() const;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
