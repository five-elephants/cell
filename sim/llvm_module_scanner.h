#pragma once

#include "ir/module_scanner.h"

#include "sim/llvm_namespace.h"


namespace sim {

  class Llvm_module_scanner : public ir::Module_scanner<Llvm_impl> {
    public:
      Llvm_module_scanner(Llvm_module& mod);

    protected:
      virtual bool insert_function(ast::Function_def const& func); 
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

