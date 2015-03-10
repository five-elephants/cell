#include "llvm_module_scanner.h"

#include "sim/llvm_function_scanner.h"
#include "ir/find.hpp"
#include "ir/find_hierarchy.h"
#include "ir/builtins.h"


#include <iostream>


namespace sim {

  using Module_scanner = ir::Module_scanner<Llvm_impl>;


  Llvm_module_scanner::Llvm_module_scanner(Llvm_module& mod)
    : Module_scanner(mod) {
    using namespace llvm;

    // prepare module type
    auto lib = find_library(mod);
    auto hier_name = hierarchical_name(mod, "mod");
    mod.impl.mod_type = llvm::StructType::create(lib->impl.context, hier_name);

    // create ctor
    //mod.impl.ctor = Function::Create(
        //FunctionType::get(mod.impl.mod_type, false),
        //Function::ExternalLinkage,
        //hierarchical_name(mod, "ctor"),
        //lib->impl.module.get());

    // register scanner callback functions
    this->template on_leave_if_type<ast::Module_def>(&Llvm_module_scanner::leave_module);
  }


  bool
  Llvm_module_scanner::insert_function(ast::Function_def const& node) {
    std::cout << "inserting function" << std::endl;

    std::shared_ptr<Llvm_function> func = create_function(node);

    m_ns.functions[func->name] = func;

    // code generation at leave of module node
    m_todo_functions.push_back(std::make_tuple(func, &node));

    return false;
  }


  bool
  Llvm_module_scanner::insert_object(ast::Variable_def const& node) {
    auto obj = create_object(node);
    m_mod.objects[obj->name] = obj;

    obj->impl.struct_index = m_member_types.size();

    m_member_types.push_back(obj->type->impl.type);

    return false;
  }


  bool
  Llvm_module_scanner::insert_process(ast::Process const& node) {
    auto proc = create_process(node);
    m_mod.processes.push_back(proc);

    auto func = std::make_shared<Llvm_function>();
    func->name = "__process__";
    func->return_type = ir::Builtins<Llvm_impl>::types.at("unit");
    func->within_module = true;
    m_todo_functions.push_back(std::make_tuple(func, &node));
    proc->function = func;

    return false;
  }


  bool
  Llvm_module_scanner::insert_periodic(ast::Periodic const& node) {
    auto period = dynamic_cast<ast::Phys_literal const&>(node.period());
    auto value = period.value();
    auto unit = dynamic_cast<ast::Identifier const&>(period.unit()).identifier();
    ir::Time::Unit tu;

    if( unit == "ps" ) tu = ir::Time::ps;
    else if( unit == "ns" ) tu = ir::Time::ns;
    else if( unit == "us" ) tu = ir::Time::us;
    else if( unit == "ms" ) tu = ir::Time::ms;
    else if( unit == "s" ) tu = ir::Time::s;
    else
      throw std::runtime_error("Unknown time unit");

    auto per = std::make_shared<ir::Periodic<Llvm_impl>>();
    per->period = ir::Time(value, tu);

    m_mod.periodicals.push_back(per);

    auto func = std::make_shared<Llvm_function>();
    func->name = "__periodic__";
    func->return_type = ir::Builtins<Llvm_impl>::types.at("unit");
    func->within_module = true;
    m_todo_functions.push_back(std::make_tuple(func, &node));
    per->function = func;

    return false;
  }


  bool
  Llvm_module_scanner::insert_once(ast::Once const& node) {
    auto period = dynamic_cast<ast::Phys_literal const&>(node.time());
    auto value = period.value();
    auto unit = dynamic_cast<ast::Identifier const&>(period.unit()).identifier();
    ir::Time::Unit tu;

    if( unit == "ps" ) tu = ir::Time::ps;
    else if( unit == "ns" ) tu = ir::Time::ns;
    else if( unit == "us" ) tu = ir::Time::us;
    else if( unit == "ms" ) tu = ir::Time::ms;
    else if( unit == "s" ) tu = ir::Time::s;
    else
      throw std::runtime_error("Unknown time unit");

    auto once = std::make_shared<ir::Once<Llvm_impl>>();
    once->time = ir::Time(value, tu);
    m_mod.onces.push_back(once);

    auto func = std::make_shared<Llvm_function>();
    func->name = "__once__";
    func->return_type = ir::Builtins<Llvm_impl>::types.at("unit");
    func->within_module = true;
    m_todo_functions.push_back(std::make_tuple(func, &node));
    once->function = func;

    return false;
  }


  bool
  Llvm_module_scanner::insert_recurrent(ast::Recurrent const& node) {
    auto rec = std::make_shared<Llvm_recurrent>();
    rec->time_id = dynamic_cast<ast::Identifier const&>(node.time_id()).identifier();
    m_mod.recurrents.push_back(rec);

    auto param = std::make_shared<Llvm_object>();
    param->name = rec->time_id;
    param->type = ir::Builtins<Llvm_impl>::types.at("int");

    auto func = std::make_shared<Llvm_function>();
    func->name = "__recurrent__";
    func->return_type = ir::Builtins<Llvm_impl>::types.at("int");
    func->parameters.push_back(param);
    func->within_module = true;
    m_todo_functions.push_back(std::make_tuple(func, &node));
    rec->function = func;

    return false;
  }


  bool
  Llvm_module_scanner::leave_module(ast::Module_def const& node) {
    m_mod.impl.mod_type->setBody(m_member_types);

    for(auto f : m_todo_functions) {
      Llvm_function_scanner scanner(m_mod, *std::get<0>(f));
      std::get<1>(f)->accept(scanner);
    }

    m_todo_functions.clear();

    return true;
  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
