#include "llvm_function_scanner.h"

#include "ir/find.hpp"
#include "ir/find_hierarchy.h"
#include "ir/builtins.h"


namespace sim {

  Llvm_function_scanner::Llvm_function_scanner(Llvm_namespace& ns, Llvm_function& function)
    : m_ns(ns),
      m_function(function),
      m_builder(llvm::getGlobalContext()) {
    init_function();
    init_scanner();
  }


  Llvm_function_scanner::Llvm_function_scanner(Llvm_module& mod, Llvm_function& function)
    : m_ns(mod),
      m_mod(&mod),
      m_function(function),
      m_builder(llvm::getGlobalContext()) {
    init_function();
    init_scanner();
  }


  void
  Llvm_function_scanner::init_function() {
    using namespace llvm;

    auto lib = ir::find_library(m_ns);
    auto name = ir::hierarchical_name(m_ns, m_function.name);

    std::cout << "creating function '" << name << "'" << std::endl;

    // create function type
    m_function.impl.func_type = get_function_type(m_function);

    // create function itself
    m_function.impl.code = Function::Create(m_function.impl.func_type,
        Function::ExternalLinkage,
        name,
        lib->impl.module.get());

    // name arguments
    auto arg_i = m_function.impl.code->arg_begin();
    if( m_mod ) {
      arg_i->setName("this_out");
      (++arg_i)->setName("this_in");
      (++arg_i)->setName("read_mask");
    }

    for(auto arg_name : m_function.parameters) {
      (++arg_i)->setName(arg_name->name);
      m_named_types[arg_name->name] = arg_name->type;
    }

    // create function entry code 
    auto bb = BasicBlock::Create(getGlobalContext(), "entry", m_function.impl.code);
    m_builder.SetInsertPoint(bb);

    // prepare symbol table with arguments
    for(auto i = m_function.impl.code->arg_begin();
        i != m_function.impl.code->arg_end();
        ++i) {
      auto ptr = m_builder.CreateAlloca(i->getType(), 0, i->getName());
      auto v = m_builder.CreateStore(i, ptr);
      m_named_values[i->getName().str()] = ptr;
    }
    
    // create function body
    auto bb_body = BasicBlock::Create(getGlobalContext(), "body", m_function.impl.code);
    m_builder.CreateBr(bb_body);
    m_builder.SetInsertPoint(bb_body);

    // set target type
    m_type_targets.push_back(m_function.return_type);
  }


  void
  Llvm_function_scanner::init_scanner() {
    this->template on_leave_if_type<ast::Return_statement>(&Llvm_function_scanner::insert_return);
    this->template on_enter_if_type<ast::Variable_ref>(&Llvm_function_scanner::insert_variable_ref);
    this->template on_visit_if_type<ast::Literal<int>>(&Llvm_function_scanner::insert_literal_int);
    this->template on_leave_if_type<ast::Op_equal>(&Llvm_function_scanner::insert_op_equal);
    this->template on_leave_if_type<ast::Op_plus>(&Llvm_function_scanner::insert_op_plus);
    this->template on_enter_if_type<ast::Assignment>(&Llvm_function_scanner::enter_assignment);
    this->template on_leave_if_type<ast::Assignment>(&Llvm_function_scanner::leave_assignment);
    this->template on_leave_if_type<ast::Compound>(&Llvm_function_scanner::leave_compound);
    this->template on_enter_if_type<ast::If_statement>(&Llvm_function_scanner::enter_if_statement);
    this->template on_leave_if_type<ast::Function_call>(&Llvm_function_scanner::leave_function_call);
    this->template on_leave_if_type<ast::Function_def>(&Llvm_function_scanner::leave_function_def);
  }


  llvm::FunctionType*
  Llvm_function_scanner::get_function_type(Llvm_function const& function) const {
    using namespace llvm;

    auto lib = ir::find_library(m_ns);
    std::vector<Type*> args;

    if( m_mod ) {
      auto r = PointerType::getUnqual(m_mod->impl.mod_type);
      args.push_back(r);
      args.push_back(r);
      args.push_back(PointerType::getUnqual(read_mask_type()));
    }

    for(auto p : function.parameters) {
      args.push_back(p->type->impl.type);
    }

    return FunctionType::get(function.return_type->impl.type,
        args,
        false);
  }


  llvm::ArrayType*
  Llvm_function_scanner::read_mask_type() const { 
    if( m_mod ) {
      auto lib = ir::find_library(m_ns);
      auto mod_type = m_mod->impl.mod_type;
      return llvm::ArrayType::get(llvm::IntegerType::get(lib->impl.context, 1),
          mod_type->getNumElements());
    } else {
      throw std::runtime_error("read_mask makes only sense for functions within modules, but m_mod == nullptr");
    }

    return nullptr;
  }

  
  bool
  Llvm_function_scanner::insert_return(ast::Return_statement const& node) {
    auto v = m_values.at(node.objects()[0]);
    m_values[&node] = m_builder.CreateRet(v);
    return true;
  }


  bool
  Llvm_function_scanner::insert_variable_ref(ast::Variable_ref const& node) {
    auto id = dynamic_cast<ast::Identifier const&>(node.identifier());

    // load value
    auto p = m_named_values.find(id.identifier());

    if( p != m_named_values.end() ) {
      m_values[&node] = m_builder.CreateLoad(p->second, "loadtmp");
    }

    // propagate type
    auto ty = m_named_types.at(id.identifier());
    m_types[&node] = ty;

    return true; 
  }


  bool
  Llvm_function_scanner::insert_literal_int(ast::Literal<int> const& node) {
    using namespace llvm;

    auto v = ConstantInt::get(getGlobalContext(), 
        APInt(64, node.value(), true));
    auto ty = ir::Builtins<Llvm_impl>::types.at("int");
    m_values[&node] = v;
    m_types[&node] = ty;

    return true;
  }



  bool
  Llvm_function_scanner::insert_op_equal(ast::Op_equal const& node) {
    insert_bin_op(node, "==");
    return true;
  }


  bool
  Llvm_function_scanner::insert_op_plus(ast::Op_plus const& node) {
    insert_bin_op(node, "+");
    return true;
  }


  bool
  Llvm_function_scanner::enter_assignment(ast::Assignment const& node) {
    std::cout << "enter_assignment" << std::endl;
    auto target_id = dynamic_cast<ast::Identifier const&>(node.identifier());

    // propagate type
    auto ty = m_named_types.at(target_id.identifier());
    m_types[&target_id] = ty;
    m_types[&node] = ty;

    m_type_targets.push_back(ty);
    return true;
  }


  bool
  Llvm_function_scanner::leave_assignment(ast::Assignment const& node) {
    std::cout << "leave_assignment" << std::endl;
    auto target_id = dynamic_cast<ast::Identifier const&>(node.identifier());

    // get right-side value
    auto rval = m_values.at(&(node.expression()));

    // store value
    auto p = m_named_values.at(target_id.identifier());
    auto lval = m_builder.CreateStore(rval, p);
    m_values[&node] = lval;

    m_type_targets.pop_back();
    return true;
  }


  bool
  Llvm_function_scanner::leave_compound(ast::Compound const& node) {
    if( node.return_last() ) {
      m_values[&node] = m_values.at(node.statements().back());
      m_types[&node] = m_types.at(node.statements().back());
    } else {
      auto ty = ir::Builtins<Llvm_impl>::types.at("unit");
      m_values[&node] = llvm::Constant::getNullValue(ty->impl.type);
      m_types[&node] = ty;
    }

    return true;
  }


  bool
  Llvm_function_scanner::enter_if_statement(ast::If_statement const& node) {
    using namespace llvm;

    m_type_targets.push_back(ir::Builtins<Llvm_impl>::types.at("bool"));
    node.condition().accept(*this);
    m_type_targets.pop_back();


    // get condition result and create basic blocks
    auto cond_val = m_values.at(&(node.condition()));
    auto bb_true = BasicBlock::Create(getGlobalContext(), "if_true", m_function.impl.code);
    auto bb_false = BasicBlock::Create(getGlobalContext(), "if_false", m_function.impl.code);
    auto bb_resume = BasicBlock::Create(getGlobalContext(), "if_resume", m_function.impl.code);

    // create conditional branch instruction
    m_builder.CreateCondBr(cond_val, bb_true, bb_false);
    
    // generate code for true branch
    m_builder.SetInsertPoint(bb_true);
    node.body().accept(*this);
    m_builder.CreateBr(bb_resume);
    bb_true = m_builder.GetInsertBlock();
    auto val_true = m_values[&(node.body())];

    // check matching result types
    auto res_ty = m_types[&(node.body())];

    // generate code for false branch
    m_builder.SetInsertPoint(bb_false);
    Value* val_false = nullptr;
    if( node.has_else_body() ) {
      node.else_body().accept(*this);
      val_false = m_values[&(node.else_body())];

      if( res_ty != m_types[&(node.else_body())] )
        throw std::runtime_error("if expression does not have matching types in both branches");
    } else {
      val_false = Constant::getNullValue(res_ty->impl.type);
    }
    m_builder.CreateBr(bb_resume);
    bb_false = m_builder.GetInsertBlock();

    m_builder.SetInsertPoint(bb_resume);

    // insert phi instruction
    auto pn = m_builder.CreatePHI(res_ty->impl.type, 2, "iftmp");
    pn->addIncoming(val_true, bb_true);
    pn->addIncoming(val_false, bb_false);

    m_types[&node] = res_ty;
    m_values[&node] = pn;

    return false;
  }


  bool
  Llvm_function_scanner::leave_function_call(ast::Function_call const& node) {
    auto& callee_id = dynamic_cast<ast::Identifier const&>(node.identifier());

    std::vector<llvm::Value*> args;

    // find function
    auto func = ir::find_function(m_ns, callee_id.identifier());
    if( !func ) {
      std::stringstream strm;
      strm << "Unable to find function '" << callee_id.identifier()
        << "' to call ("
        << __func__
        << ")";
      throw std::runtime_error(strm.str());
    }

    // add module arguments
    if( func->within_module ) {
      auto this_out_v = m_builder.CreateLoad(m_named_values.at("this_out"),
          "this_out_tmp");
      args.push_back(this_out_v);
      
      auto this_in_v = m_builder.CreateLoad(m_named_values.at("this_in"),
          "this_in_tmp");
      args.push_back(this_in_v);

      auto read_mask_v = m_builder.CreateLoad(m_named_values.at("read_mask"),
          "read_mask_tmp");
      args.push_back(read_mask_v);
    }

    // add parameter values
    for(auto i : node.expressions()) {
      args.push_back(m_values.at(i));
    }

    // insert function call
    auto v = m_builder.CreateCall(func->impl.code, args, "callres");

    m_values[&node] = v;
    m_types[&node] = func->return_type;

    return true;
  }


  bool
  Llvm_function_scanner::leave_function_def(ast::Function_def const& node) {
    auto v = m_values.at(&(node.body()));
    m_values[&node] = m_builder.CreateRet(v);
    m_type_targets.pop_back();

    return true;
  }
}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
