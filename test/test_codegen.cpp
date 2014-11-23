#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#include "parse_driver.h"
#include "sim/scan_ast.h"
#include "sim/llvm_namespace_scanner.h"
#include "ast/ast_printer.h"


TEST(Codegen_test, empty_module) {
  Parse_driver driver;
  if( driver.parse("test/simulator_test/empty_module.mini") )
    throw std::runtime_error("parse failed");

  ir::Library<sim::Llvm_impl> lib;

  lib.name = "main";
  lib.ns = std::make_shared<sim::Llvm_namespace>();

  ast::Ast_printer printer(std::cout);
  driver.ast_root().accept(printer);

  sim::Llvm_namespace_scanner scanner(*lib.ns, lib);
  driver.ast_root().accept(scanner);
}



/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

