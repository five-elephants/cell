#include "llvm_function_scanner.h"

#include "ir/find.hpp"
#include "ir/find_hierarchy.h"


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
  }


  void
  Llvm_function_scanner::init_scanner() {
    this->template on_leave_if_type<ast::Return_statement>(&Llvm_function_scanner::insert_return);
    this->template on_enter_if_type<ast::Variable_ref>(&Llvm_function_scanner::insert_variable_ref);
    this->template on_visit_if_type<ast::Literal<int>>(&Llvm_function_scanner::insert_literal_int);
    this->template on_leave_if_type<ast::Op_equal>(&Llvm_function_scanner::insert_op_equal);
    this->template on_enter_if_type<ast::Assignment>(&Llvm_function_scanner::enter_assignment);
    this->template on_leave_if_type<ast::Assignment>(&Llvm_function_scanner::leave_assignment);
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
    m_values[&node] = v;

    return true;
  }


  bool
  Llvm_function_scanner::insert_op_equal(ast::Op_equal const& node) {
    auto ret_ty = ir::Builtins<Llvm_impl>::types["bool"];

    // check with expected target type
    if( !m_type_targets.empty() ) {
      auto ty_target = m_types.at(m_type_targets.back());
      if( ty_target != ret_ty ) {
        std::stringstream strm;
        strm << node.location() << ": expected type '"
          << ty_target->name
          << "' but operator == returns 'bool' instead";
        throw std::runtime_error(strm.str());
      }
    }

    // get types and values
    auto ty_left = m_types.at(&(node.left()));
    auto ty_right = m_types.at(&(node.right()));
    auto v_left = m_values.at(&(node.left()));
    auto v_right = m_values.at(&(node.right()));

    //cout << "insert_op_equal: ["
      //<< ty_left->name
      //<< "] == ["
      //<< ty_right->name
      //<< "] -> [bool]" << endl;

    // select an operator
    std::shared_ptr<Llvm_operator> op = ir::find_operator(m_ns, "==", ret_ty, ty_left, ty_right);
    if( op ) {
      auto v_cmp = op->impl.insert_func(m_builder, v_left, v_right);
      m_values[&node] = v_cmp;
      m_types[&node] = ret_ty;
    } else {
      std::stringstream strm;
      strm << node.location() << ": failed to find operator '==' with signature: ["
        << ty_left->name
        << "] == [" 
        << ty_right->name
        << "] -> ["
        << ret_ty->name
        << "]";
      throw std::runtime_error(strm.str());
    }

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

    m_type_targets.push_back(&target_id);
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

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
