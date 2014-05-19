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
  engine.teardown();
}
