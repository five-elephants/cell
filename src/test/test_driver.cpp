#include "sim/simulation_engine.h"
#include "logging/logger.h"

#include <gtest/gtest.h>


class Test_driver : public ::testing::Test {
  protected:
    virtual void SetUp() {
      init_logging();

      m_logger = log4cxx::Logger::getLogger("cell.test");
    }

    log4cxx::LoggerPtr m_logger;
};

void test_driver_function(ir::Time const& t,
    sim::Runset::Module_frame this_in,
    sim::Runset::Module_frame this_out,
    sim::Runset::Module_frame this_prev) {
  auto logger = log4cxx::Logger::getLogger("cell.test");
  LOG4CXX_INFO(logger, "test_driver_function called at " << t);
}

TEST_F(Test_driver, add_driver_function) {
  sim::Simulation_engine engine("../lib/test/driver.cell", "m");

  engine.setup();
  engine.add_driver(test_driver_function, "");
  engine.simulate(ir::Time(100, ir::Time::ns));

  engine.teardown();
}


struct Test_driver_struct {
  sim::Module_inspector insp;
  log4cxx::LoggerPtr logger;
  int64_t a = 0;

  Test_driver_struct(sim::Module_inspector insp)
    : insp(insp) {
    logger = log4cxx::Logger::getLogger("cell.test.driver");
    logger->setLevel(log4cxx::Level::getTrace());
    log4cxx::Logger::getLogger("cell.sim")->setLevel(log4cxx::Level::getTrace());
  }

  void operator () (ir::Time const& t,
      sim::Runset::Module_frame this_in,
      sim::Runset::Module_frame this_out,
      sim::Runset::Module_frame this_prev) {
    insp.set<int64_t>("a", a++);
    insp.set<int64_t>("b", 1);
    LOG4CXX_INFO(logger, "driver called: y = " << insp.get<int64_t>("y"));
    LOG4CXX_DEBUG(logger, "driver.a = " << a);
  }
};


TEST_F(Test_driver, add_driver_struct) {
  sim::Simulation_engine engine("../lib/test/driver.cell", "m");

  engine.setup();
  Test_driver_struct driver(engine.inspect_module(""));
  engine.add_driver(driver, "");
  engine.simulate(ir::Time(100, ir::Time::ns));

  EXPECT_EQ(driver.a - 1, driver.insp.get<int64_t>("a"));
  EXPECT_EQ(1, driver.insp.get<int64_t>("b"));
  EXPECT_EQ(driver.a, driver.insp.get<int64_t>("y"));

  engine.teardown();
}


