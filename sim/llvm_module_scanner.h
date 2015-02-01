#pragma once

#include "ir/module_scanner.h"

#include "sim/llvm_namespace.h"


namespace sim {

  class Llvm_module_scanner : public ir::Module_scanner<Llvm_impl> {
    public:
      Llvm_module_scanner(Llvm_module& mod);

    protected:
      std::vector<llvm::Type*> m_member_types;

      virtual bool insert_function(ast::Function_def const& func); 
      virtual bool insert_object(ast::Variable_def const& var);
      virtual bool leave_module(ast::Module_def const& node);
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

