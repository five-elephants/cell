#include "logging/logger.h"
#include "sim/simulation_engine.h"
#include "sim/vcd_instrumenter.h"
#include "method/driver.h"
#include "method/observer_channel.h"
#include "driver.h"


namespace std {  // necessary for log4cxx
  ostream& operator << (ostream& os, cell::s const& x) {
    return os << "s("
      << "clk: " << x.clk << ", "
      << "a:" << x.a
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

    cell::s drive(ir::Time const& t, cell::s port) {
      port.a = 1;
      port.b = 2;
      return port;
    }

    void observe(ir::Time const& t, cell::s port) {
      m_hit = true;
    }

    bool m_hit = false;
};


class My_observer : public method::Observer_channel<My_observer, cell::s> {
  public:
    bool pass;

    My_observer()
      : Observer_channel() {
      pass = true;
      m_first = true;
      //m_logger->setLevel(log4cxx::Level::getTrace());
    }

    bool clocked(cell::s port, cell::s prev) const {
      LOG4CXX_DEBUG(m_logger, "port: " << port << " prev: " << prev);
      return port.clk && !prev.clk;
    }

    void cycle(cell::s port) {
      if( !m_first ) {
        if( port.y != port.a + port .b ) {
          LOG4CXX_ERROR(m_logger, "assertion failed: "
              << port.y << " == " << port.a << " + " << port.b
              << " does not hold.");
          pass = false;
        }
      } else
        m_first = false;
    }

  private:
    bool m_first;
};


class Tb_driver {
  public:
    Tb_driver(std::string const& source,
        std::string const& top,
        std::string const& trace_file = "")
      : m_pass(true),
        m_engine(source, top) {
      if( !trace_file.empty() ) {
        m_instrumenter.reset(new sim::Vcd_instrumenter(trace_file));
        m_engine.instrument(*m_instrumenter);
      }

      m_engine.setup();
      m_engine.add_driver(m_driver, "");
      m_engine.add_driver(m_my_observer, "");
    }

    void run() {
      m_engine.simulate(ir::Time(100, ir::Time::ns));

      if( !m_driver.m_hit )
        m_pass = false;

      if( !m_my_observer.pass )
        m_pass = false;
    }

    ~Tb_driver() {
      m_engine.teardown();
    }

    bool passed() const { return m_pass; }

  private:
    bool m_pass;
    sim::Instrumented_simulation_engine m_engine;
    std::unique_ptr<sim::Vcd_instrumenter> m_instrumenter;
    My_driver m_driver;
    My_observer m_my_observer;
};


int main(int argc, char* argv[]) {
  init_logging();

  Tb_driver tb("../lib/test/driver.cell", "m", "tb_driver.vcd");
  tb.run();

  if( tb.passed() )
    return 0;
  else
    return 1;
}

