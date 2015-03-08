#include "sim/simulation_engine.h"
#include "sim/vcd_instrumenter.h"

#include <gtest/gtest.h>


TEST(Demos, lif_neuron) {
  sim::Instrumented_simulation_engine engine("test/simulator_test/demo_lif.cell",
      "demo.lif_neuron");
  sim::Vcd_instrumenter instr("demos__lif_neuron.vcd");

  engine.instrument(instr);
  engine.setup();
  engine.simulate(ir::Time(1000, ir::Time::ms));
  engine.teardown();
}
