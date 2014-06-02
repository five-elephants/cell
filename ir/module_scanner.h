#pragma once

#include "ir/namespace_scanner.h"


namespace ir {

  class Module_scanner : public Namespace_scanner {
    public:
      Module_scanner(Module& mod, Codegen_if& codegen)
        : Namespace_scanner(mod, codegen),
          m_mod(mod) {
      }

      virtual bool enter(ast::Node_if const& node); 


    private:
      Module& m_mod;

      //std::shared_ptr<Function> insert_function(ast::Function_def const& func);
      std::shared_ptr<Object> insert_object(ast::Variable_def const& node);
      std::shared_ptr<Instantiation> insert_instantiation(ast::Module_instantiation const& node);
      std::shared_ptr<Process> insert_process(ast::Process const& node);
      std::shared_ptr<Process> insert_periodic(ast::Periodic const& node);

      virtual std::shared_ptr<Codeblock_if> make_codeblock();
  };

}
