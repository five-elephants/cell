#include "sim/codegen_visitor.h"

#include "ast/ast.h"

#include <iostream>
#include <sstream>

#define VISITOR_METHOD(x) \
  bool \
  Codegen_visitor:: x (ast::Node_if const& node)

namespace sim {

  using namespace llvm;

  Codegen_visitor::Codegen_visitor(ir::Namespace const& ns, Llvm_codeblock& block)
    : Visitor_base(),
      m_ns(ns),
      m_codeblock(block) {

#define ENTER(x, y) m_mappings_enter[&typeid(ast:: x )] = &Codegen_visitor:: y 
#define VISIT(x, y) m_mappings_visit[&typeid(ast:: x )] = &Codegen_visitor:: y 
#define LEAVE(x, y) m_mappings_leave[&typeid(ast:: x )] = &Codegen_visitor:: y 

    ENTER(Compound, compound_enter); 
    ENTER(Variable_def, var_def);
    VISIT(Literal<int>, literal_int);
    VISIT(Identifier, identifier);
    LEAVE(Assignment, assignment);
    LEAVE(Compound, compound_leave);
    LEAVE(Op_plus, op_plus);
    LEAVE(Return_statement, return_statement);

#undef ENTER 
#undef VISIT 
#undef LEAVE 
  }


  bool
  Codegen_visitor::enter(ast::Node_if const& node) {
    std::type_info const* node_type = &typeid(node);

    std::cout  << std::string(m_indent*2, ' ')
      << "enter " << node_type->name() << std::endl;
    ++m_indent;

    auto mapping = m_mappings_enter.find(node_type);
    if( mapping != m_mappings_enter.end() ) {
      return mapping->second(*this, node);
    }

    return true;
  }
  

  bool
  Codegen_visitor::visit(ast::Node_if const& node) {
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
  Codegen_visitor::leave(ast::Node_if const& node) {
    std::type_info const* node_type = &typeid(node);

    std::cout << std::string(m_indent*2, ' ')
      << "leave " << node_type->name() << std::endl;
    --m_indent;

    auto mapping = m_mappings_leave.find(node_type);
    if( mapping != m_mappings_leave.end() ) {
      return mapping->second(*this, node);
    }

    return true;
  }


  VISITOR_METHOD(compound_enter) {
    //auto bb = BasicBlock::Create(m_codeblock.m_context, "compound", m_codeblock.m_function);
    //m_codeblock.m_builder.SetInsertPoint(bb);
    //m_values[&node] = bb;
    return true;
  }


  VISITOR_METHOD(compound_leave) {
    auto c = dynamic_cast<ast::Compound const&>(node);
    std::cout << "compound leave"
      << " searching value for " << std::hex << c.statements().back() << std::dec
      << std::endl;
    m_values[&node] = m_values.at(c.statements().back());
    return true;
  }


  VISITOR_METHOD(var_def) {
    // this is always inside a function
    auto def = dynamic_cast<ast::Variable_def const&>(node);
    auto id = dynamic_cast<ast::Identifier const&>(def.identifier());
    auto type_id = dynamic_cast<ast::Identifier const&>(def.type());

    // TODO value initialization 
    //Value* v = m_codeblock.make_constant(type_id.identifier(), 0);
    //m_values[&node] = m_named_values[id.identifier()] = v;

    auto v = m_codeblock.allocate_variable(id.identifier(), 
        type_id.identifier());
    m_named_values[id.identifier()] = v;
    m_values[&node] = v;

    return false;
  }


  VISITOR_METHOD(return_statement) {
    auto r = dynamic_cast<ast::Return_statement const&>(node);
    Value* v = m_values.at(r.objects()[0]);
    m_values[&node] = m_codeblock.m_builder.CreateRet(v);
    std::cout << "value for 0x" << std::hex << &node << std::dec << std::endl;
    return true;
  }
  

  VISITOR_METHOD(assignment) {
    auto a = dynamic_cast<ast::Assignment const&>(node);
    auto target_name = dynamic_cast<ast::Identifier const&>(a.identifier()).identifier();

    auto it = m_named_values.find(target_name);
    if( it != m_named_values.end() ) {
      m_values[&node] = m_codeblock.m_builder.CreateStore(m_values.at(&a.expression()),
          it->second);
    } else {
      llvm::Value* ptr = m_codeblock.get_global(target_name);
      if( !ptr ) {
        std::stringstream strm;
        strm << node.location() << ": object '" << target_name << "' is unknown";
        throw std::runtime_error(strm.str());
      }
      m_values[&node] = m_codeblock.m_builder.CreateStore(m_values.at(&a.expression()), ptr);
    }

    return true;
  }


  VISITOR_METHOD(op_plus) {
    auto op = dynamic_cast<ast::Op_plus const&>(node);

    std::cout << "op_plus" << std::endl;
    llvm::Value* left = m_values.at(&op.left());
    llvm::Value* right = m_values.at(&op.right());

    m_values[&node] = m_codeblock.m_builder.CreateAdd(left, right, "addtmp");

    return true;
  }


  VISITOR_METHOD(literal_int) {
    std::cout << "make literal_int" << std::endl;
    auto lit = dynamic_cast<ast::Literal<int> const&>(node);
    Value* v = m_codeblock.make_constant("int", lit.value());
    if( !v ) {
      std::stringstream strm;
      strm << node.location() << ": use of unknown type 'int'";
      throw std::runtime_error(strm.str());
    }
    m_values[&node] = v;
    return true;
  }


  VISITOR_METHOD(identifier) {
    auto id = dynamic_cast<ast::Identifier const&>(node);
    auto p = m_named_values.find(id.identifier());
    if( p != m_named_values.end() ) {
      m_values[&node] = m_codeblock.m_builder.CreateLoad(p->second, "loadtmp");
    } else {
      Value* v = m_codeblock.get_global(id.identifier());
      if( v == nullptr ) {
        std::stringstream strm;
        strm << node.location() << ": named value '" << id.identifier() << "' not found";
        throw std::runtime_error(strm.str());
      }

      m_values[&node] = m_codeblock.m_builder.CreateLoad(v, "loadtmp");
      //m_values[&node] = v;
    }

    return true;
  }

}