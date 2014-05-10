#include "sim/module_codegen_visitor.h"

#include "ast/ast.h"

#include <sstream>
#include <stdexcept>

#define VISITOR_METHOD(x) \
  bool \
  Module_codegen_visitor:: x (ast::Node_if const& node)

namespace sim {

  using namespace llvm;

  Module_codegen_visitor::Module_codegen_visitor(Llvm_codeblock& block)
    : Visitor_base(),
      m_codeblock(block) {
#define ENTER(x, y) m_mappings_enter[&typeid(ast:: x )] = &Module_codegen_visitor:: y 
#define VISIT(x, y) m_mappings_visit[&typeid(ast:: x )] = &Module_codegen_visitor:: y 
#define LEAVE(x, y) m_mappings_leave[&typeid(ast:: x )] = &Module_codegen_visitor:: y 

    ENTER(Module_def, mod_def);
    ENTER(Variable_def, var_def);
    ENTER(Function_def, func_def);

#undef ENTER 
#undef VISIT 
#undef LEAVE 
  }

  bool
  Module_codegen_visitor::enter(ast::Node_if const& node) {
    bool rv = false;
    std::type_info const* node_type = &typeid(node);

    std::cout  << std::string(m_indent*2, ' ')
      << "enter " << node_type->name() << " {" << std::endl;
    ++m_indent;

    auto mapping = m_mappings_enter.find(node_type);
    if( mapping != m_mappings_enter.end() ) {
      rv = mapping->second(*this, node);
    }

    if( m_root )
      m_root = false;

    return rv;
  }
  

  bool
  Module_codegen_visitor::visit(ast::Node_if const& node) {
    std::type_info const* node_type = &typeid(node);

    std::cout << std::string(m_indent*2, ' ')
      << "visit " << node_type->name() << std::endl;

    auto mapping = m_mappings_visit.find(node_type);
    if( mapping != m_mappings_visit.end() ) {
      return mapping->second(*this, node);
    }

    return true;
  }


  bool
  Module_codegen_visitor::leave(ast::Node_if const& node) {
    std::type_info const* node_type = &typeid(node);

    std::cout << std::string(m_indent*2, ' ')
      << "leave " << node_type->name() << '\n';
    --m_indent;
    std::cout << std::string(m_indent*2, ' ')
      << "}"
      << std::endl;

    auto mapping = m_mappings_leave.find(node_type);
    if( mapping != m_mappings_leave.end() ) {
      return mapping->second(*this, node);
    }

    return true;
  }


  VISITOR_METHOD(mod_def) {
    return m_root;
  }


  VISITOR_METHOD(var_def) {
    auto& def = dynamic_cast<ast::Variable_def const&>(node);
    auto& id = dynamic_cast<ast::Identifier const&>(def.identifier());
    auto& type_id = dynamic_cast<ast::Identifier const&>(def.type());

    auto type = m_codeblock.get_type(type_id.identifier());
    if( !type ) {
      std::stringstream strm;
      strm << node.location() << ": type '" << type_id.identifier() << "' not found";
      throw std::runtime_error(strm.str());
    }
    std::cout << "found type " << type_id.identifier() << std::endl;
    m_types.push_back(type);

    return false;
  }


  VISITOR_METHOD(func_def) {
    auto& def = dynamic_cast<ast::Function_def const&>(node);
    auto& id = dynamic_cast<ast::Identifier const&>(def.identifier());

    if( id.identifier() == "__init__" )
      m_has_init = true;

    return false;
  }


  Type*
  Module_codegen_visitor::get_module_type(ir::Label const& name) {
    auto rv = StructType::create(m_codeblock.m_context, m_types, name);
    return rv;
  }


  Value*
  Module_codegen_visitor::get_initialization(llvm::Value* obj_ptr) {
    auto rv = m_codeblock.m_builder.CreateLoad(obj_ptr, "obj");
    return rv;
  }


}

#undef VISITOR_METHOD


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
