#pragma once

#include "ir/namespace_scanner.h"


namespace ir {

  class Module_scanner : public Namespace_scanner {
    public:
      Module_scanner(Module& mod);

      virtual bool enter(ast::Node_if const& node); 


    protected:
      Module& m_mod;

      virtual bool insert_function(ast::Function_def const& func);
      virtual bool insert_socket(ast::Socket_def const& sock);
      //std::shared_ptr<Function> insert_function(ast::Function_def const& func);
      virtual bool insert_object(ast::Variable_def const& node);
      virtual bool insert_instantiation(ast::Module_instantiation const& node);
      virtual bool insert_process(ast::Process const& node);
      virtual bool insert_periodic(ast::Periodic const& node);
  };

}
