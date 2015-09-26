#pragma once

#include "ir/namespace.h"
#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "sim/llvm_codeblock.h"

#include <unordered_map>
#include <functional>
#include <typeinfo>

namespace sim {

  class Codegen_visitor : public ast::Scanner_base<Codegen_visitor> {
    public:
      Codegen_visitor(ir::Namespace const& ns, Llvm_codeblock& block);

      virtual bool compound_enter(ast::Compound const& node);
      virtual bool compound_leave(ast::Compound const& node);
      virtual bool var_def(ast::Variable_def const& node);
      virtual bool return_statement(ast::Return_statement const& node);
      virtual bool assignment(ast::Assignment const& node);
      virtual bool op_plus(ast::Op_plus const& node);
      virtual bool literal_int(ast::Literal<int> const& node);
      virtual bool literal_string(ast::Literal<std::string> const& node);
      virtual bool var_ref(ast::Variable_ref const& node);
      virtual bool function_call(ast::Function_call const& node);

      void add_named_value(std::string const& name, llvm::AllocaInst* value) {
        m_named_values[name] = value;
      }
            

    private:
      typedef std::unordered_map<std::type_info const*,
          std::function<bool(Codegen_visitor&, ast::Node_if const& node)>> Node_type_map;
      typedef std::unordered_map<ast::Node_if const*, llvm::Value*> Node_value_map;
      typedef std::unordered_map<std::string, llvm::AllocaInst*> Name_value_map;

      ir::Namespace const& m_ns;
      Llvm_codeblock& m_codeblock;

      Node_value_map m_values;
      Name_value_map m_named_values;

      unsigned int m_indent = 0;
  };

}

/* vim: set et fenc=utf-8 ff=unix sts=0 sw=2 ts=2 : */
