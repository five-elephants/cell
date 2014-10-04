#pragma once

#include "ir/namespace_scanner.h"

#include "sim/llvm_namespace.h"


namespace sim {

  class Llvm_namespace_scanner : public ir::Namespace_scanner<Llvm_impl> {
    public:
      Llvm_namespace_scanner(Llvm_namespace& ns, Llvm_library& library);

    protected:
      Llvm_library& m_library;
      Llvm_namespace& m_llvm_ns;

      virtual bool insert_function(ast::Function_def const& func); 
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
