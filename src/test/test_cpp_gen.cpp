#include <gtest/gtest.h>

#include "sim/cpp_gen.h"
#include "sim/simulation_engine.h"
#include "logging/logger.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <iostream>


class Test_cpp_gen : public ::testing::Test {
  protected:
    static bool is_set_up;

    virtual void SetUp() {
      if( !is_set_up ) {
        init_logging();
        is_set_up = true;
      }
      m_logger = log4cxx::Logger::getLogger("cell.test");
    }

    log4cxx::LoggerPtr m_logger;
};

bool Test_cpp_gen::is_set_up;


TEST_F(Test_cpp_gen, write_ir) {
  auto int_ty = std::make_shared<sim::Llvm_type>();
  int_ty->name = "int";

  auto ty = std::make_shared<sim::Llvm_type>();
  ty->name = "foo";

  auto elem_a = std::make_shared<sim::Llvm_port>();
  elem_a->name = "a";
  elem_a->direction = ir::Direction::Input;
  elem_a->type = int_ty;
  ty->elements["a"] = elem_a;

  std::stringstream strm;
  sim::write_cpp(strm, ty);

  EXPECT_EQ("struct foo {\n\tint64_t a;\n};\n", strm.str());
}


TEST_F(Test_cpp_gen, write_ir_nested) {
  auto int_ty = std::make_shared<sim::Llvm_type>();
  int_ty->name = "int";
  auto bool_ty = std::make_shared<sim::Llvm_type>();
  bool_ty->name = "bool";

  auto foo = std::make_shared<sim::Llvm_type>();
  foo->name = "foo";

  auto bar = std::make_shared<sim::Llvm_type>();
  bar->name = "bar";

  auto elem_a = std::make_shared<sim::Llvm_port>();
  elem_a->name = "a";
  elem_a->direction = ir::Direction::Input;
  elem_a->type = int_ty;

  auto elem_clk = std::make_shared<sim::Llvm_port>();
  elem_clk->name = "clk";
  elem_clk->direction = ir::Direction::Input;
  elem_clk->type = bool_ty;

  auto elem_bar = std::make_shared<sim::Llvm_port>();
  elem_bar->name = "b";
  elem_bar->direction = ir::Direction::Input;
  elem_bar->type = bar;

  bar->elements["clk"] = elem_clk;
  foo->elements["a"] = elem_a;
  foo->elements["b"] = elem_bar;

  std::stringstream strm;
  sim::write_cpp(strm, foo);

  EXPECT_EQ(
      "struct bar {"
      "\n\tbool clk;"
      "\n};\n"
      "struct foo {"
      "\n\tint64_t a;"
      "\n\tbar b;"
      "\n};\n",
      strm.str()
  );
}


TEST_F(Test_cpp_gen, cpp_name) {
  EXPECT_EQ("hallo_welt", sim::cpp_name("hallo.welt"));
  EXPECT_EQ("foo", sim::cpp_name("foo"));
}


TEST_F(Test_cpp_gen, from_code) {
  sim::Simulation_engine engine("../lib/test/driver.cell", "m");
  std::stringstream strm;

  engine.setup();
  auto insp = engine.inspect_module("");
  sim::write_cpp(strm, insp.module()->socket);
  engine.teardown();

  EXPECT_EQ("struct s {\n\tint64_t a;\n\tint64_t b;\n\tbool clk;\n\tint64_t y;\n};\n",
      strm.str());
}


