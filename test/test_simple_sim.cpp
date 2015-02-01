#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>
#include "sim/simulation_engine.h"
//#include "sim/stream_instrumenter.h"

#include <gtest/gtest.h>

class Simulator_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      //llvm::InitializeNativeTarget();
      //ir::Builtins::init();
    }
};


//TEST_F(Simulator_test, basic_process) {
  //sim::Simulation_engine engine("test/simulator_test/basic_process.mini", "test.basic_process");

  //engine.setup();
  //auto intro = engine.inspect_module("test.basic_process");

  //EXPECT_EQ(intro.get<int64_t>("a"), 1);

  //engine.simulate(ir::Time(10, ir::Time::ns));

  //EXPECT_EQ(intro.get<int64_t>("a"), 1);
  //EXPECT_EQ(intro.get<int64_t>("b"), 2);
  //EXPECT_EQ(intro.get<int64_t>("c"), 3);

  //engine.teardown();
//}


//TEST_F(Simulator_test, basic_periodic) {
  //sim::Simulation_engine engine("test/simulator_test/basic_periodic.mini", "test.basic_periodic");

  //engine.setup();
  //engine.simulate(ir::Time(10, ir::Time::ns));

  //auto intro = engine.inspect_module("test.basic_periodic");
  //auto counter = intro.get<int64_t>("counter");
  //auto acc = intro.get<int64_t>("acc");

  //EXPECT_EQ(counter, acc);

  //engine.teardown();
//}


TEST_F(Simulator_test, empty_module) {
  sim::Simulation_engine engine("test/simulator_test/empty_module.mini", "test.empty_module");

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));
  engine.teardown();
}


TEST_F(Simulator_test, functions) {
  sim::Simulation_engine engine("test/simulator_test/functions.cell", "m");

  engine.setup();

  auto insp = engine.inspect_module("m");

  auto addf = insp.get_function_ptr<int(char*,char*,char*,int,int)>("add");

  ASSERT_EQ(addf(nullptr, nullptr, nullptr, 1, 2), 3);
  ASSERT_EQ(addf(nullptr, nullptr, nullptr, 0, 0), 0);
  ASSERT_EQ(addf(nullptr, nullptr, nullptr, -1, 15), 14);

  engine.teardown();
}


//TEST_F(Simulator_test, basic_logging) {
  //sim::Instrumented_simulation_engine engine("test/simulator_test/basic_periodic.mini", "test.basic_periodic");

  //std::stringstream strm;
  //sim::Stream_instrumenter instr(strm);
  //engine.instrument(instr);

  //engine.setup();
  //engine.simulate(ir::Time(10, ir::Time::ns));
  //engine.teardown();

  //std::cout << "Loggin output:\n"
    //<< strm.str() << std::endl;
//}


//TEST_F(Simulator_test, basic_array) {
  //sim::Simulation_engine engine("test/simulator_test/basic_array.mini", "test.basic_array");

  //engine.setup();
  //engine.simulate(ir::Time(10, ir::Time::ns));
  //engine.teardown();  
//}
