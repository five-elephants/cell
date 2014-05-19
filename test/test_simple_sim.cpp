#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>
#include "sim/simulation_engine.h"

#include <gtest/gtest.h>

class Simulator_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      llvm::InitializeNativeTarget();
      ir::Builtins::init();
    }
};


TEST_F(Simulator_test, basic_process) {
  sim::Simulation_engine engine("test/simulator_test/basic_process.mini", "test.basic_process");

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));

  auto intro = engine.inspect_module("test.basic_process");
  EXPECT_EQ(intro.get<int64_t>("a"), 1);
  EXPECT_EQ(intro.get<int64_t>("b"), 2);
  EXPECT_EQ(intro.get<int64_t>("c"), 3);

  engine.teardown();
}


TEST_F(Simulator_test, basic_periodic) {
  sim::Simulation_engine engine("test/simulator_test/basic_periodic.mini", "test.basic_periodic");

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));

  auto intro = engine.inspect_module("test.basic_periodic");
  auto counter = intro.get<int64_t>("counter");
  auto acc = intro.get<int64_t>("acc");

  EXPECT_EQ(counter, acc);

  engine.teardown();
}


TEST_F(Simulator_test, empty_module) {
  sim::Simulation_engine engine("test/simulator_test/empty_module.mini", "test.empty_module");

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));
  engine.teardown();
}



