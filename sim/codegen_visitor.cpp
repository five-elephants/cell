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
    auto bb = BasicBlock::Create(m_codeblock.m_context, "compound", m_codeblock.m_function);
    m_codeblock.m_builder.SetInsertPoint(bb);
    return true;
  }


  VISITOR_METHOD(compound_leave) {
    auto c = dynamic_cast<ast::Compound const&>(node);
    std::cout << "compound leave"
      << " searching value for " << std::hex << c.statements().back() << std::dec
      << std::endl;
    Value* v = m_values.at(c.statements().back());
    m_values[&node] = v;
    return true;
  }


  VISITOR_METHOD(var_def) {
    auto def = dynamic_cast<ast::Variable_def const&>(node);
    auto id = dynamic_cast<ast::Identifier const&>(def.identifier());
    auto type_id = dynamic_cast<ast::Identifier const&>(def.type());

    Value* v = ConstantInt::get(m_codeblock.m_context,
       APInt(64, 0, true)); 
    m_values[&node] = m_named_values[id.identifier()] = v;

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

    m_values[&node] = m_named_values[target_name] = m_values.at(&a.expression());

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
    llvm::Value* v = llvm::ConstantInt::get(m_codeblock.m_context,
        llvm::APInt(64, lit.value(), true));
    m_values[&node] = v;
    //m_codeblock.m_builder.Insert(v);
    return true;
  }


  VISITOR_METHOD(identifier) {
    auto id = dynamic_cast<ast::Identifier const&>(node);
    auto p = m_named_values.find(id.identifier());
    if( p != m_named_values.end() ) {
      Value* v = m_named_values.at(id.identifier());
      m_values[&node] = v;
    } else {
      std::stringstream strm;
      strm << node.location() << ": named value '" << id.identifier() << "' not found";
      throw std::runtime_error(strm.str());
    }

    return true;
  }

}
