#include "sim/codegen_visitor.h"

#include "ast/ast.h"

#include <iostream>
#include <sstream>


namespace sim {

  using namespace llvm;

  Codegen_visitor::Codegen_visitor(ir::Namespace const& ns, Llvm_codeblock& block)
    : m_ns(ns),
      m_codeblock(block) {

    on_enter_if_type<ast::Variable_def>(&Codegen_visitor::var_def);
    on_enter_if_type<ast::Variable_ref>(&Codegen_visitor::var_ref);
    on_visit_if_type<ast::Literal<int>>(&Codegen_visitor::literal_int);
    on_visit_if_type<ast::Literal<std::string>>(&Codegen_visitor::literal_string);
    on_leave_if_type<ast::Function_call>(&Codegen_visitor::function_call);
    on_leave_if_type<ast::Assignment>(&Codegen_visitor::assignment);
    on_leave_if_type<ast::Op_plus>(&Codegen_visitor::op_plus);
    on_leave_if_type<ast::Return_statement>(&Codegen_visitor::return_statement);
  }


  bool
  Codegen_visitor::compound_enter(ast::Compound const& node) {
    //auto bb = BasicBlock::Create(m_codeblock.m_context, "compound", m_codeblock.m_function);
    //m_codeblock.m_builder.SetInsertPoint(bb);
    //m_values[&node] = bb;
    return true;
  }


  bool
  Codegen_visitor::compound_leave(ast::Compound const& c) {
    //std::cout << "compound leave"
      //<< " searching value for " << std::hex << c.statements().back() << std::dec
      //<< std::endl;
    m_values[&c] = m_values.at(c.statements().back());
    return true;
  }


  bool
  Codegen_visitor::var_def(ast::Variable_def const& def) {
    // this is always inside a function
    auto id = dynamic_cast<ast::Identifier const&>(def.identifier());

    if( typeid(def.type()) == typeid(ast::Identifier) ) {
      auto type_id = dynamic_cast<ast::Identifier const&>(def.type());

      // TODO value initialization 
      //Value* v = m_codeblock.make_constant(type_id.identifier(), 0);
      //m_values[&node] = m_named_values[id.identifier()] = v;

      auto v = m_codeblock.allocate_variable(id.identifier(), 
          type_id.identifier());
      m_named_values[id.identifier()] = v;
      m_values[&def] = v;
    } else if( typeid(def.type()) == typeid(ast::Array_type) ) {
      throw std::runtime_error("not yet implemented");
    }

    return false;
  }


  bool
  Codegen_visitor::return_statement(ast::Return_statement const& r) {
    Value* v = m_values.at(r.objects()[0]);
    m_values[&r] = m_codeblock.m_builder.CreateRet(v);
    std::cout << "value for 0x" << std::hex << &r << std::dec << std::endl;
    return true;
  }
  

  bool
  Codegen_visitor::assignment(ast::Assignment const& a) {
    auto target_name = dynamic_cast<ast::Identifier const&>(a.identifier()).identifier();

    auto it = m_named_values.find(target_name);
    if( it != m_named_values.end() ) {
      m_values[&a] = m_codeblock.m_builder.CreateStore(m_values.at(&a.expression()),
          it->second);
    } else {
      llvm::Value* ptr = m_codeblock.get_module_object_out(target_name);
      if( !ptr ) {
        std::stringstream strm;
        strm << a.location() << ": object '" << target_name << "' is unknown in current module";
        throw std::runtime_error(strm.str());
      }
      m_values[&a] = m_codeblock.m_builder.CreateStore(m_values.at(&a.expression()), ptr);
    }

    return true;
  }


  bool
  Codegen_visitor::op_plus(ast::Op_plus const& op) {
    std::cout << "op_plus" << std::endl;
    llvm::Value* left = m_values.at(&op.left());
    llvm::Value* right = m_values.at(&op.right());

    m_values[&op] = m_codeblock.m_builder.CreateAdd(left, right, "addtmp");

    return true;
  }


  bool
  Codegen_visitor::literal_int(ast::Literal<int> const& lit) {
    std::cout << "make literal_int" << std::endl;
    Value* v = m_codeblock.make_constant("int", lit.value());
    if( !v ) {
      std::stringstream strm;
      strm << lit.location() << ": use of unknown type 'int'";
      throw std::runtime_error(strm.str());
    }
    m_values[&lit] = v;
    return true;
  }


  bool
  Codegen_visitor::literal_string(ast::Literal<std::string> const& lit) {
    std::cout << "make literal_string" << std::endl;
    Value* v = m_codeblock.make_constant("string", lit.value());
    if( !v ) {
      std::stringstream strm;
      strm << lit.location() << ": use of unknown type 'string'";
      throw std::runtime_error(strm.str());
    }
    m_values[&lit] = v;
    return true;
  }


  // TODO I need multiple types of identifiers for functions/variables/types etc.
  bool
  Codegen_visitor::var_ref(ast::Variable_ref const& ref) {
    auto id = dynamic_cast<ast::Identifier const&>(ref.identifier());
    auto p = m_named_values.find(id.identifier());
    if( p != m_named_values.end() ) {
      m_values[&ref] = m_codeblock.m_builder.CreateLoad(p->second, "loadtmp");
    } else {
      Value* v = m_codeblock.get_module_object_in(id.identifier());
      if( v == nullptr ) {
        std::stringstream strm;
        strm << ref.location() << ": named value '" << id.identifier() << "' not found";
        throw std::runtime_error(strm.str());
      }

      m_values[&ref] = m_codeblock.m_builder.CreateLoad(v, "loadtmp");
      
      // register the read access
      Value* read_mask = m_codeblock.get_read_mask(id.identifier());
      if( read_mask == nullptr ) {
        std::stringstream strm;
        strm << ref.location() << ": failed to get read_mask entry for named value '"
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


  bool
  Codegen_visitor::function_call(ast::Function_call const& call) {
    auto& callee_id = dynamic_cast<ast::Identifier const&>(call.identifier());

    std::vector<Value*> args;
    for(auto i : call.expressions()) {
      args.push_back(m_values.at(i));
    }
    auto v = m_codeblock.create_function_call(callee_id.identifier(), args);
    if( !v ) {
      std::stringstream strm;
      strm << call.location() << ": could not call function '"
        << callee_id.identifier()
        << "', please try again in a few minutes...";
      throw std::runtime_error(strm.str());
    }

    m_values[&call] = v;

    return true;
  }

}
