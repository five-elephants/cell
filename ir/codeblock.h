#pragma once

#include "namespace.h"
#include "ast/node_if.h"

namespace ir {

  class Codeblock_if {
    public:
      /** Process the AST to generate the code for this codeblock. */
      virtual void scan_ast(ast::Node_if const& tree) = 0;

      /** Process the AST to generate toplevel code for a module. */
      virtual void scan_ast_module(ast::Node_if const& tree) = 0;

      /** Append to the list of predefined objects accessible in the codeblock */
      virtual void append_predefined_objects(std::map<Label, std::shared_ptr<Object>> objects) = 0;

      /** Give this codeblock a function prototype */
      virtual void prototype(std::shared_ptr<Function> func) = 0;

      virtual void process(std::shared_ptr<Process> proc) = 0;

      /** This codeblock lives in a module */
      virtual void enclosing_module(Module* mod) = 0;
  };


  class Codeblock_base : public Codeblock_if {
    public:
      virtual ~Codeblock_base();
  };


  class Null_codeblock : public Codeblock_base {
    public:
      virtual void scan_ast(ast::Node_if const& tree);
      virtual void scan_ast_module(ast::Node_if const& tree);

      virtual void append_predefined_objects(std::map<Label, std::shared_ptr<Object>> objects);

      virtual void prototype(std::shared_ptr<Function> func);
      virtual void process(std::shared_ptr<Process> proc);
      virtual void enclosing_module(Module* mod);
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
