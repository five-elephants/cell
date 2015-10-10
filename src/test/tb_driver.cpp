#include "logging/logger.h"
#include "sim/simulation_engine.h"
#include "driver.h"


struct Driver {
  bool hit = false;

  void operator () (ir::Time const& t,
      sim::Runset::Module_frame this_in,
      sim::Runset::Module_frame this_out,
      sim::Runset::Module_frame this_prev) {
    hit = true;
  }
};


class Tb_driver {
  public:
    Tb_driver(std::string const& source, std::string const& top)
      : m_pass(true),
        m_engine(source, top) {
      m_engine.setup();
      m_engine.add_driver(m_driver, "");
    }

    void run() {
      m_engine.simulate(ir::Time(100, ir::Time::ns));

      if( !m_driver.hit )
        m_pass = false;
    }

    ~Tb_driver() {
      m_engine.teardown();
    }

    bool passed() const { return m_pass; }

  private:
    bool m_pass;
    sim::Simulation_engine m_engine;
    Driver m_driver;
};


int main(int argc, char* argv[]) {
  init_logging();
  Tb_driver tb("../lib/test/driver.cell", "m");

  if( tb.passed() )
    return 0;
  else
    return 1;
}

