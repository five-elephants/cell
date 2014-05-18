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

    // TODO compounds: push/pop named value environments
    //ENTER(Compound, compound_enter); 
    ENTER(Variable_def, var_def);
    ENTER(Variable_ref, var_ref);
    VISIT(Literal<int>, literal_int);
    VISIT(Literal<std::string>, literal_string);
    LEAVE(Function_call, function_call);
    LEAVE(Assignment, assignment);
    //LEAVE(Compound, compound_leave);
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
      << "enter " << node_type->name() << " {" << std::endl;
    ++m_indent;

    auto mapping = m_mappings_enter.find(node_type);
    if( mapping != m_mappings_enter.end() ) {
      try {
        return mapping->second(*this, node);
      } catch( std::out_of_range const& err ) {
        std::cerr << node.location()
          << ": Encountered out_of_range error. Probably a missing "
             "value for a node of the AST.\n"
             "Node is of type: "
          << node_type->name()
          << "\nwhat(): " << err.what()
          << std::endl;
        throw err;
      }
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
      try {
        return mapping->second(*this, node);
      } catch( std::out_of_range const& err ) {
        std::cerr << node.location()
          << ": Encountered out_of_range error. Probably a missing "
             "value for a node of the AST.\n"
             "Node is of type: "
          << node_type->name()
          << "\nwhat(): " << err.what()
          << std::endl;
        throw err;
      }
    }

    return true;
  }


  bool
  Codegen_visitor::leave(ast::Node_if const& node) {
    std::type_info const* node_type = &typeid(node);

    std::cout << std::string(m_indent*2, ' ')
      << "leave " << node_type->name() << '\n';
    --m_indent;
    std::cout << std::string(m_indent*2, ' ')
      << "}"
      << std::endl;

    auto mapping = m_mappings_leave.find(node_type);
    if( mapping != m_mappings_leave.end() ) {
      try {
        return mapping->second(*this, node);
      } catch( std::out_of_range const& err ) {
        std::cerr << node.location()
          << ": Encountered out_of_range error. Probably a missing "
             "value for a node of the AST.\n"
             "Node is of type: "
          << node_type->name()
          << "\nwhat(): " << err.what()
          << std::endl;
        throw err;
      }
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
      llvm::Value* ptr = m_codeblock.get_module_object_out(target_name);
      if( !ptr ) {
        std::stringstream strm;
        strm << node.location() << ": object '" << target_name << "' is unknown in current module";
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


  VISITOR_METHOD(literal_string) {
    std::cout << "make literal_string" << std::endl;
    auto& lit = dynamic_cast<ast::Literal<std::string> const&>(node);
    Value* v = m_codeblock.make_constant("string", lit.value());
    if( !v ) {
      std::stringstream strm;
      strm << node.location() << ": use of unknown type 'string'";
      throw std::runtime_error(strm.str());
    }
    m_values[&node] = v;
    return true;
  }


  // TODO I need multiple types of identifiers for functions/variables/types etc.
  VISITOR_METHOD(var_ref) {
    auto ref = dynamic_cast<ast::Variable_ref const&>(node);
    auto id = dynamic_cast<ast::Identifier const&>(ref.identifier());
    auto p = m_named_values.find(id.identifier());
    if( p != m_named_values.end() ) {
      m_values[&node] = m_codeblock.m_builder.CreateLoad(p->second, "loadtmp");
    } else {
      Value* v = m_codeblock.get_module_object_in(id.identifier());
      if( v == nullptr ) {
        std::stringstream strm;
        strm << node.location() << ": named value '" << id.identifier() << "' not found";
        throw std::runtime_error(strm.str());
      }

      m_values[&node] = m_codeblock.m_builder.CreateLoad(v, "loadtmp");
      
      // register the read access
      Value* read_mask = m_codeblock.get_read_mask(id.identifier());
      if( read_mask == nullptr ) {
        std::stringstream strm;
        strm << node.location() << ": failed to get read_mask entry for named value '"
          << id.identifier()
          << "'";
        throw std::runtime_error(strm.str());
      }

      m_codeblock.m_builder.CreateStore(
          ConstantInt::get(m_codeblock.m_context, APInt(1, 1, false)),
          read_mask
      );
    }

    return true;
  }


  VISITOR_METHOD(function_call) {
    auto& call = dynamic_cast<ast::Function_call const&>(node);
    auto& callee_id = dynamic_cast<ast::Identifier const&>(call.identifier());

    std::vector<Value*> args;
    for(auto i : call.expressions()) {
      args.push_back(m_values.at(i));
    }
    auto v = m_codeblock.create_function_call(callee_id.identifier(), args);
    if( !v ) {
      std::stringstream strm;
      strm << node.location() << ": could not call function '"
        << callee_id.identifier()
        << "', please try again in a few minutes...";
      throw std::runtime_error(strm.str());
    }

    m_values[&node] = v;

    return true;
  }

}
