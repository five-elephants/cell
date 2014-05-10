#pragma once

#include "ir/namespace.h"
#include "ast/visitor.h"
#include "sim/llvm_codeblock.h"

#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <vector>

namespace sim {

  class Module_codegen_visitor : public ast::Visitor_base {
    public:
      Module_codegen_visitor(Llvm_codeblock& block);

      virtual bool enter(ast::Node_if const& node);
      virtual bool visit(ast::Node_if const& node);
      virtual bool leave(ast::Node_if const& node);

      virtual bool mod_def(ast::Node_if const& node);
      virtual bool var_def(ast::Node_if const& node);
      virtual bool func_def(ast::Node_if const& node);

      llvm::Type* get_module_type(ir::Label const& name);
      llvm::Value* get_initialization(llvm::Value* obj);
      bool has_init() { return m_has_init; }

    private:
      typedef std::unordered_map<std::type_info const*,
          std::function<bool(Module_codegen_visitor&, ast::Node_if const& node)>> Node_type_map;
      typedef std::unordered_map<ast::Node_if const*, llvm::Value*> Node_value_map;
      typedef std::vector<llvm::Type*> Type_list;

      Llvm_codeblock& m_codeblock;
      bool m_root = true;

      Node_type_map m_mappings_enter;
      Node_type_map m_mappings_visit;
      Node_type_map m_mappings_leave;

      Node_value_map m_values;
      Type_list m_types;
      bool m_has_init = false;

      unsigned int m_indent = 0;
  };

}

/* vim: set et fenc=utf-8 ff=unix sts=0 sw=2 ts=2 : */

