#pragma once

#include "ir/namespace.h"
#include "ast/visitor.h"
#include "sim/llvm_codeblock.h"

#include <unordered_map>
#include <functional>
#include <typeinfo>

namespace sim {

  class Codegen_visitor : public ast::Visitor_base {
    public:
      Codegen_visitor(ir::Namespace const& ns, Llvm_codeblock& block);

      virtual bool enter(ast::Node_if const& node);
      virtual bool visit(ast::Node_if const& node);
      virtual bool leave(ast::Node_if const& node);

      virtual bool compound_enter(ast::Node_if const& node);
      virtual bool compound_leave(ast::Node_if const& node);
      virtual bool var_def(ast::Node_if const& node);
      virtual bool return_statement(ast::Node_if const& node);
      virtual bool assignment(ast::Node_if const& node);
      virtual bool op_plus(ast::Node_if const& node);
      virtual bool literal_int(ast::Node_if const& node);
      virtual bool identifier(ast::Node_if const& node);

      void add_named_value(std::string const& name, llvm::Value* value) {
        m_named_values[name] = value;
      }
            

    private:
      typedef std::unordered_map<std::type_info const*,
          std::function<bool(Codegen_visitor&, ast::Node_if const& node)>> Node_type_map;
      typedef std::unordered_map<ast::Node_if const*, llvm::Value*> Node_value_map;
      typedef std::unordered_map<std::string, llvm::Value*> Name_value_map;

      ir::Namespace const& m_ns;
      Llvm_codeblock& m_codeblock;

      Node_type_map m_mappings_enter;
      Node_type_map m_mappings_visit;
      Node_type_map m_mappings_leave;

      Node_value_map m_values;
      Name_value_map m_named_values;

      unsigned int m_indent = 0;
  };

}

/* vim: set et fenc=utf-8 ff=unix sts=0 sw=2 ts=2 : */
