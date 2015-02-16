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
    mod.impl.ctor = Function::Create(
        FunctionType::get(mod.impl.mod_type, false),
        Function::ExternalLinkage,
        hierarchical_name(mod, "ctor"),
        lib->impl.module.get());

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
    auto per = std::make_shared<ir::Periodic<Llvm_impl>>();
    per->period = ir::Time(2, ir::Time::ns);

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
