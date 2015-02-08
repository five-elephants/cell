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
  using namespace std::placeholders;

  sim::Simulation_engine engine("test/simulator_test/functions.cell", "m");

  engine.setup();

  auto insp = engine.inspect_module("m");

  auto addf = std::bind(insp.get_function_ptr<int(char*,char*,char*,int,int)>("add"),
      nullptr,
      nullptr,
      nullptr,
      _1,
      _2);

  ASSERT_EQ(addf(1, 2), 3);
  ASSERT_EQ(addf(0, 0), 0);
  ASSERT_EQ(addf(-1, 15), 14);


  auto testf = std::bind(insp.get_function_ptr<bool(char*,char*,char*,int,int)>("test"),
      nullptr,
      nullptr,
      nullptr,
      _1,
      _2);

  ASSERT_EQ(testf(0, 0), true);
  ASSERT_EQ(testf(0, 1), false);
  ASSERT_EQ(testf(-500, 2), false);


  auto condf = std::bind(insp.get_function_ptr<int(char*,char*,char*,bool,int,int)>("cond"),
      nullptr,
      nullptr,
      nullptr,
      _1,
      _2,
      _3);

  ASSERT_EQ(condf(true, 1, 2), 1);
  ASSERT_EQ(condf(false, 1, 2), 2);


  auto cond2f = std::bind(insp.get_function_ptr<int(char*,char*,char*,bool,int)>("cond2"),
      nullptr,
      nullptr,
      nullptr,
      _1,
      _2);

  ASSERT_EQ(cond2f(true, 3), 6);
  ASSERT_EQ(cond2f(false, 3), 3);

  auto facf = std::bind(insp.get_function_ptr<int(char*,char*,char*,int)>("fac"),
      nullptr,
      nullptr,
      nullptr,
      _1);

  {
    int f = 1;
    for(int i=0; i<10; i++) {
      ASSERT_EQ(facf(i), f);
      f *= i+1;
    }
  }

  engine.teardown();
}


TEST_F(Simulator_test, module_access) {
  using namespace std::placeholders;

  sim::Simulation_engine engine("test/simulator_test/function_in_module.mini",
    "m");

  engine.setup();
  auto insp = engine.inspect_module("m");

  auto set_a = std::bind(insp.get_function_ptr<void(char*,char*,char*,int)>("set_a"),
      nullptr,
      nullptr,
      nullptr,
      _1);

  auto get_a = std::bind(insp.get_function_ptr<int(char*,char*,char*)>("get_a"),
      nullptr,
      nullptr,
      nullptr);

  set_a(5);
  ASSERT_EQ(get_a(), 5);

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
