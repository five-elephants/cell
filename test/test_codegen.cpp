#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#include "parse_driver.h"
#include "sim/scan_ast.h"
#include "sim/llvm_namespace_scanner.h"
#include "ast/ast_printer.h"
#include "ir/builtins.h"


TEST(Codegen_test, empty_module) {
  using namespace std;

  Parse_driver driver;
  if( driver.parse("test/simulator_test/function_in_module.mini") )
    throw std::runtime_error("parse failed");

  auto lib = std::make_shared<ir::Library<sim::Llvm_impl>>();

  lib->name = "main";
  lib->ns = std::make_shared<sim::Llvm_namespace>();
  lib->ns->enclosing_library = lib;
  lib->impl = sim::create_library_impl(lib->name);

  // init builtins
  auto& context = lib->impl.context;
  ir::Builtins<sim::Llvm_impl>::types.at("int")->impl.type = llvm::Type::getInt64Ty(context);
  ir::Builtins<sim::Llvm_impl>::types.at("void")->impl.type = llvm::Type::getVoidTy(context);
  ir::Builtins<sim::Llvm_impl>::types.at("string")->impl.type = llvm::TypeBuilder<char*, false>::get(context);


  // print AST
  ast::Ast_printer printer(std::cout);
  driver.ast_root().accept(printer);

  // generate code
  sim::Llvm_namespace_scanner scanner(*(lib->ns));
  driver.ast_root().accept(scanner);

  // show generated code
  cout << "Generated code:\n=====\n";
  lib->impl.module->dump();
  cout << "\n====="
    << endl;
}



/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

