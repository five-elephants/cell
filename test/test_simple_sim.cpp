#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>
#include "sim/simulation_engine.h"
#include "sim/stream_instrumenter.h"
#include "sim/vcd_instrumenter.h"

#include <gtest/gtest.h>

class Simulator_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      //llvm::InitializeNativeTarget();
      //ir::Builtins::init();
    }
};


TEST_F(Simulator_test, basic_process) {
  sim::Simulation_engine engine("test/simulator_test/basic_process.mini",
      "test.basic_process");

  engine.setup();
  auto intro = engine.inspect_module("test.basic_process");

  EXPECT_EQ(1, intro.get<int64_t>("a"));

  engine.simulate(ir::Time(10, ir::Time::ns));

  EXPECT_EQ(1, intro.get<int64_t>("a"));
  EXPECT_EQ(2, intro.get<int64_t>("b"));
  EXPECT_EQ(3, intro.get<int64_t>("c"));

  engine.teardown();
}


TEST_F(Simulator_test, basic_periodic) {
  sim::Simulation_engine engine("test/simulator_test/basic_periodic.mini", "test.basic_periodic");

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));

  auto intro = engine.inspect_module("test.basic_periodic");
  auto counter = intro.get<int64_t>("counter");
  auto acc = intro.get<int64_t>("acc");

  EXPECT_EQ(acc, counter);

  engine.teardown();
}


TEST_F(Simulator_test, basic_fsm) {
  sim::Simulation_engine engine("test/simulator_test/basic_fsm.cell", "test");

  engine.setup();
  auto intro = engine.inspect_module("test");

  auto ctr = intro.get<int64_t>("ctr");
  auto state = intro.get<int64_t>("state");

  std::cout << "Module data members:\n";
  for(auto obj : intro.module()->objects) {
    std::cout << "   " << obj.first << '\n';
  }
  std::cout << std::endl;

  EXPECT_EQ(0, ctr);
  EXPECT_EQ(0, state);

  engine.simulate(ir::Time(40, ir::Time::ns));

  ctr = intro.get<int64_t>("ctr");
  state = intro.get<int64_t>("state");

  EXPECT_EQ(11, ctr);
  EXPECT_EQ(3, state);

  engine.teardown();
}


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

  int tmp;
  bool tmpb;
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

  insp.call(tmp, "add", 1, 2);
  ASSERT_EQ(3, tmp);


  auto testf = std::bind(insp.get_function_ptr<bool(char*,char*,char*,int,int)>("test"),
      nullptr,
      nullptr,
      nullptr,
      _1,
      _2);

  ASSERT_EQ(testf(0, 0), true);
  ASSERT_EQ(testf(0, 1), false);
  ASSERT_EQ(testf(-500, 2), false);

  insp.call(tmpb, "test", 0, 0);
  ASSERT_EQ(tmpb, true);
  insp.call(tmpb, "test", -500, 2);
  ASSERT_EQ(tmpb, false);


  auto condf = std::bind(insp.get_function_ptr<int(char*,char*,char*,bool,int,int)>("cond"),
      nullptr,
      nullptr,
      nullptr,
      _1,
      _2,
      _3);

  ASSERT_EQ(condf(true, 1, 2), 1);
  ASSERT_EQ(condf(false, 1, 2), 2);

  insp.call(tmp, "cond", true, 1, 2);
  ASSERT_EQ(tmp, 1);


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

      insp.call(tmp, "fac", i);
      ASSERT_EQ(f, tmp);

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

  insp.call("set_a", 5);
  int a;
  insp.call(a, "get_a");
  ASSERT_EQ(5, a);

  engine.teardown();
}

TEST_F(Simulator_test, basic_logging) {
  sim::Instrumented_simulation_engine engine("test/simulator_test/basic_periodic.mini", "test.basic_periodic");

  std::stringstream strm;
  sim::Stream_instrumenter instr(strm);
  engine.instrument(instr);

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));
  engine.teardown();

  std::cout << "Loggin output:\n"
    << strm.str() << std::endl;
}

TEST_F(Simulator_test, vcd_logging) {
  sim::Instrumented_simulation_engine engine("test/simulator_test/basic_periodic.mini", "test.basic_periodic");

  sim::Vcd_instrumenter instr("simulator_test__vcd_logging.vcd");
  engine.instrument(instr);

  engine.setup();
  engine.simulate(ir::Time(10, ir::Time::ns));
  engine.teardown();
}



//TEST_F(Simulator_test, basic_array) {
  //sim::Simulation_engine engine("test/simulator_test/basic_array.mini", "test.basic_array");

  //engine.setup();
  //engine.simulate(ir::Time(10, ir::Time::ns));
  //engine.teardown();  
//}
