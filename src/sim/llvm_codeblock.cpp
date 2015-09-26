#include "sim/llvm_codeblock.h"

#include "sim/codegen_visitor.h"
#include "sim/module_codegen_visitor.h"

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/TypeBuilder.h>
#include <sstream>
#include <stdexcept>

namespace sim {

  using namespace llvm;

  Llvm_codeblock::Llvm_codeblock(Llvm_codegen& parent,
        llvm::LLVMContext& context,
        llvm::IRBuilder<>& builder,
        std::shared_ptr<llvm::Module> module,
        ir::Namespace const& ns)
    : Codeblock_base(),
      m_codegen(parent),
      m_context(context),
      m_builder(builder),
      m_module(module),
      m_enclosing_ns(ns) {
    m_function_type = FunctionType::get(Type::getVoidTy(m_context), false);
  }


  void
  Llvm_codeblock::scan_ast(ast::Node_if const& tree) {
    Codegen_visitor visitor(m_enclosing_ns, *this);

    if( (m_function_name == "__init__") && (m_enclosing_mod) ) {
      m_function = m_codegen.get_module_init(m_enclosing_mod);
    } else {
      m_function = Function::Create(m_function_type,
          Function::ExternalLinkage,
          m_function_name,
          m_module.get());
    }

    m_bb = BasicBlock::Create(m_context, "entry", m_function);
    m_builder.SetInsertPoint(m_bb);

    // make local mutable copies of the arguments
    if( m_prototype ) {
      m_codegen.register_function(m_prototype, m_function);
      
      auto i = m_function->arg_begin();
      if( m_enclosing_mod ) {
        i->setName("this_out");
        ++i;
        i->setName("this_in");
        ++i;
        i->setName("read_mask");
        ++i; 
      }

      auto j = m_prototype->parameters.begin();
      for( ; 
          (i != m_function->arg_end()) && (j != m_prototype->parameters.end()); 
          ++i, ++j) {
        i->setName((*j)->name);

        auto ptr = m_builder.CreateAlloca(i->getType(), 0, (*j)->name);
        auto v = m_builder.CreateStore(i, ptr);

        visitor.add_named_value((*j)->name, ptr);
      }
    } else if( m_process ) {
      m_codegen.register_process(m_process, m_function);

      auto i = m_function->arg_begin();
      i->setName("this_out");
      (++i)->setName("this_in");
      (++i)->setName("read_mask");

      auto mod_type = m_codegen.get_module_type(m_enclosing_mod);
      m_read_mask = m_builder.CreateAlloca(read_mask_type(), nullptr, "read_mask");
      std::vector<Constant*> zeros(mod_type->getNumElements(),
          ConstantInt::get(m_context, APInt(1, 0, false)));
      auto zero_mask = ConstantArray::get(read_mask_type(), zeros);
      m_builder.CreateStore(zero_mask, m_read_mask);
    }

    tree.accept(visitor);

    if( !m_function->back().getTerminator() ) {
      m_builder.CreateRetVoid();
    }

    if( verifyFunction(*m_function, PrintMessageAction) ) {
      std::stringstream strm;
      strm << tree.location()
        << ": during generation of function '"
        << m_function_name << "': "
        << "error during code generation. (verifyFunction returned false)";
      throw std::runtime_error(strm.str());
    }

    m_codegen.optimize(m_function);
  }


  void
  Llvm_codeblock::scan_ast_module(ast::Node_if const& tree) {
    Module_codegen_visitor visitor(*this);
    tree.accept(visitor);

    m_function_name = std::string("ctor_") + m_enclosing_mod->name;
    
    auto mod_type = visitor.get_module_type(m_enclosing_mod->name);
    m_codegen.add_module_type(m_enclosing_mod, mod_type);

    m_function_type = FunctionType::get(mod_type, false);

    m_function = Function::Create(m_function_type,
        Function::ExternalLinkage,
        m_function_name,
        m_module.get());

    m_codegen.register_module_ctor(m_enclosing_mod, m_function);

    m_bb = BasicBlock::Create(m_context, "entry", m_function);
    m_builder.SetInsertPoint(m_bb);

    // create intialization method
    auto obj_ptr = m_builder.CreateAlloca(mod_type, nullptr, "this");
    auto read_mask_ptr = m_builder.CreateAlloca(read_mask_type(), nullptr, "read_mask");
    auto ini = visitor.get_initialization(obj_ptr);
    m_builder.CreateStore(ini, obj_ptr);

    // call __init__ if defined
    if( visitor.has_init() ) {
      std::vector<Type*> arg_types{
        PointerType::getUnqual(mod_type),
        PointerType::getUnqual(mod_type),
        PointerType::getUnqual(read_mask_type())
      };
      auto init_type = FunctionType::get(Type::getVoidTy(m_context), arg_types, false);
      auto init_func = Function::Create(init_type,
          Function::ExternalLinkage,
          "__init__",
          m_module.get());

      m_builder.CreateCall3(init_func, obj_ptr, obj_ptr, read_mask_ptr);
      m_codegen.register_module_init(m_enclosing_mod, init_func);
    }

    auto rv = m_builder.CreateLoad(obj_ptr, "obj");
    m_builder.CreateRet(rv);

    if( verifyFunction(*m_function, PrintMessageAction) ) {
      std::stringstream strm;
      strm << tree.location()
        << ": during generation of constructor '"
        << m_function_name << "': "
        << "error during code generation. (verifyFunction returned false)";
      throw std::runtime_error(strm.str());
    }

    m_codegen.optimize(m_function);
  }


  void
  Llvm_codeblock::append_predefined_objects(
      std::map<ir::Label, std::shared_ptr<ir::Object>> m) {
    m_predefined_objects.insert(m.begin(), m.end());
  }


  void
  Llvm_codeblock::prototype(std::shared_ptr<ir::Function> func) {
    m_prototype = func;

    std::vector<Type*> arg_types;
    if( m_enclosing_mod ) {
      arg_types.push_back(PointerType::getUnqual(m_codegen.get_module_type(m_enclosing_mod)));
      arg_types.push_back(PointerType::getUnqual(m_codegen.get_module_type(m_enclosing_mod)));

      // create read mask argument
      arg_types.push_back(PointerType::getUnqual(read_mask_type()));
    }

    for(auto p : func->parameters) {
      arg_types.push_back(get_type(p->type->name));
    }
    m_function_type = FunctionType::get(m_codegen.get_type(func->return_type),
        arg_types,
        false);
    m_function_name = func->name;
  }


  void
  Llvm_codeblock::process(std::shared_ptr<ir::Process> proc) {
    m_process = proc;

    std::vector<Type*> arg_types;
    if( !m_enclosing_mod ) {
      throw std::runtime_error("Process must be in module");
    }

    auto mod_type = m_codegen.get_module_type(m_enclosing_mod);
    arg_types.push_back(PointerType::getUnqual(mod_type));
    arg_types.push_back(PointerType::getUnqual(mod_type));
    arg_types.push_back(PointerType::getUnqual(read_mask_type()));

    //m_function_type = FunctionType::get(read_mask_type(),
    m_function_type = FunctionType::get(Type::getVoidTy(m_context),
        arg_types,
        false);
    m_function_name = "__process__";
  }

  void
  Llvm_codeblock::enclosing_module(ir::Module* mod) {
    m_enclosing_mod = mod;
  }



  llvm::AllocaInst*
  Llvm_codeblock::allocate_variable(ir::Label const& name, ir::Label const& type_name) const {
    auto type = get_type(type_name);
    if( type ) {
      IRBuilder<> bld(&m_function->getEntryBlock(), m_function->getEntryBlock().begin());
      return bld.CreateAlloca(type, 0, name.c_str());
    }

    return nullptr;
  }


  llvm::Value*
  Llvm_codeblock::get_module_object_out(ir::Label const& name) {
    if( m_enclosing_mod ) {
      auto it = m_enclosing_mod->objects.find(name);
      if( it == m_enclosing_mod->objects.end() )
        return nullptr;

      auto index = std::distance(m_enclosing_mod->objects.begin(), it);
      auto this_out = m_function->arg_begin();

      auto rv = m_builder.CreateStructGEP(this_out, index, "obj");
      return rv;
    }

    return nullptr;
  }


  llvm::Value*
  Llvm_codeblock::get_module_object_in(ir::Label const& name) {
    if( m_enclosing_mod ) {
      auto it = m_enclosing_mod->objects.find(name);
      if( it == m_enclosing_mod->objects.end() )
        return nullptr;

      auto index = std::distance(m_enclosing_mod->objects.begin(), it);
      auto this_in = ++(m_function->arg_begin());

      auto rv = m_builder.CreateStructGEP(this_in, index, "obj");
      return rv;
    }

    return nullptr;
  }


  llvm::Value*
  Llvm_codeblock::get_read_mask(ir::Label const& name) {
    if( m_enclosing_mod ) {
      auto it = m_enclosing_mod->objects.find(name);
      if( it == m_enclosing_mod->objects.end() )
        return nullptr;

      auto index = std::distance(m_enclosing_mod->objects.begin(), it);
      auto read_mask = ++++(m_function->arg_begin());

      auto rv = m_builder.CreateConstGEP2_32(read_mask, 0, index, "obj");
      return rv;
    }

    return nullptr;
  }


  llvm::Value*
  Llvm_codeblock::create_function_call(ir::Label const& callee,
      std::vector<llvm::Value*> const& args) {
    if( m_enclosing_mod ) {
      auto the_func = ir::find_function(m_enclosing_ns, callee);
      if( !the_func )
        return nullptr;

      auto func = m_codegen.get_function(the_func);
      if( !func )
        return nullptr;

      std::vector<llvm::Value*> all_args(args.size()+2);

      if( the_func->within_module ) {
        all_args.resize(args.size() + 3);

        auto func_arg_it = m_function->arg_begin();
        all_args[0] = func_arg_it++;
        all_args[1] = func_arg_it++;
        if( m_process ) {
          all_args[2] = m_read_mask;
        } else {
          all_args[2] = func_arg_it++;
        }
        std::copy(args.begin(), args.end(), all_args.begin() + 3);
      } else {
        all_args.resize(args.size());
        std::copy(args.begin(), args.end(), all_args.begin());
      }

      auto rv = m_builder.CreateCall(func, all_args, "callres");
      return rv;
    }

    return nullptr;
  }

}

