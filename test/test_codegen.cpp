#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>

#include "parse_driver.h"
#include "sim/scan_ast.h"
#include "sim/llvm_namespace_scanner.h"
#include "ast/ast_printer.h"
#include "sim/llvm_builtins.h"



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
  init_builtins(lib);


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




TEST(Codegen_test, functions) {
  using namespace std;

  Parse_driver driver;
  if( driver.parse("test/simulator_test/functions.cell") )
    throw std::runtime_error("parse failed");

  auto lib = std::make_shared<ir::Library<sim::Llvm_impl>>();

  lib->name = "main";
  lib->ns = std::make_shared<sim::Llvm_namespace>();
  lib->ns->enclosing_library = lib;
  lib->impl = sim::create_library_impl(lib->name);

  // LLVM initialization
  llvm::InitializeNativeTarget();
  
  // init builtins
  init_builtins(lib);


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

  // execute a function
  {
    using namespace llvm;

    verifyModule(*(lib->impl.module));

    EngineBuilder exe_bld(lib->impl.module.get());
    string err_str;
    exe_bld.setErrorStr(&err_str);
    exe_bld.setEngineKind(EngineKind::JIT);
    
    ExecutionEngine* exe = exe_bld.create();
    if( !exe ) {
      stringstream strm;
      strm << "Failed to create execution engine!: " << err_str;
      throw std::runtime_error(strm.str());
    }
    exe->DisableSymbolSearching(false);

    // get a pointer to function
    auto zero_func = lib->impl.module->getFunction("main.m.zero");
    if( !zero_func )
      throw std::runtime_error("Failed to find function 'main.m.zero'");

    void* ptr = exe->getPointerToFunction(zero_func);
    int(*zerof)() = (int(*)())(ptr);

    ASSERT_EQ(zerof(), 0);
  }
}


/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

