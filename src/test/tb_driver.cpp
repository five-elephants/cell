#include "logging/logger.h"
#include "sim/simulation_engine.h"
#include "method/driver.h"
#include "driver.h"


namespace std {  // necessary for log4cxx
  ostream& operator << (ostream& os, cell::s const& x) {
    return os << "s(a:" << x.a
      << ", b:" << x.b
      << ", y:" << x.y
      << ")";
  }
}

class My_driver : public method::Driver<My_driver, cell::s> {
  public:
    My_driver()
      : Driver() {
    }

    cell::s drive(cell::s port) {
      port.a = 1;
      port.b = 2;
      return port;
    }

    void observe(cell::s port) {
      m_hit = true;
    }

    bool m_hit = false;
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

      if( !m_driver.m_hit )
        m_pass = false;
    }

    ~Tb_driver() {
      m_engine.teardown();
    }

    bool passed() const { return m_pass; }

  private:
    bool m_pass;
    sim::Simulation_engine m_engine;
    My_driver m_driver;
};


int main(int argc, char* argv[]) {
  init_logging();
  Tb_driver tb("../lib/test/driver.cell", "m");
  tb.run();

  if( tb.passed() )
    return 0;
  else
    return 1;
}

