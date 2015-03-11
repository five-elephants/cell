#include "sim/simulation_engine.h"
#include "sim/vcd_instrumenter.h"
#include "logging/logger.h"

#include <gtest/gtest.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>


class Demos : public ::testing::Test {
  protected:
    virtual void SetUp() {
      init_logging();

      m_logger = log4cxx::Logger::getLogger("cell.sim");
    }

    log4cxx::LoggerPtr m_logger;
};


TEST_F(Demos, lif_neuron) {
  sim::Instrumented_simulation_engine engine("test/simulator_test/demo_lif.cell",
      "demo.lif_neuron");
  sim::Vcd_instrumenter instr("demos__lif_neuron.vcd");

  engine.instrument(instr);
  engine.setup();
  engine.simulate(ir::Time(1000, ir::Time::ms));
  engine.teardown();
}


TEST_F(Demos, dataflow) {
  sim::Instrumented_simulation_engine engine("test/simulator_test/dataflow.cell",
      "demo.dataflow");
  sim::Vcd_instrumenter instr("dataflow.vcd");

  engine.instrument(instr);
  engine.setup();

  auto intro_dataflow = engine.inspect_module("demo.dataflow");

  LOG4CXX_INFO(m_logger, "init: add_res = "
      << intro_dataflow.get<int64_t>("add_res"));

  engine.simulate(ir::Time(100, ir::Time::ns));

  LOG4CXX_INFO(m_logger, "after sim: add_res = "
      << intro_dataflow.get<int64_t>("add_res"));

  engine.teardown();
}
