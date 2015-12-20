#pragma once

#include "logging/logger.h"
#include "sim/simulation_engine.h"

/** Verification methodology */
namespace method {

  /** Interface to execution of simulation.
   *
   * @tparam Derived The derived class for CRTP.
   * @tparam Socket Type for the socket of the targetted module. Generate with
   * cellwrap.
   *
   * Specialize this class to your needs and use it together with
   * Simulation_engine::add_driver() to register callbacks for a module during
   * the simulation cycle. In every cycle i.e. everytime processes are run
   * within this module Driver will call observe and drive function of the
   * derived class Derived. The expected function signatures are
   *
   * - void Derived::observe(ir::Time const& t, Socket port)
   * - Socket Derived::drive(ir::Time const& t, Socket port)
   *
   * In both cases port will be loaded with the contents of the this_in module
   * frame.  For drive the return value will overwrite this_out at the beginnig
   * of the cycle.
   * */
  template<typename Derived, typename Socket>
  class Driver {
    public:
      Driver()
        : m_logger(log4cxx::Logger::getLogger("tb.Driver")) {
      }

      /** Call back from sim::Simulation_engine.
       *
       * Converts raw sim::Runset::Module_frame to type Socket. */
      void operator () (ir::Time const& t,
          sim::Runset::Module_frame this_in,
          sim::Runset::Module_frame this_out,
          sim::Runset::Module_frame this_prev) {

        Socket s = from_frame(this_in);
        LOG4CXX_TRACE(m_logger, "@" << t << " in : " << s);

        static_cast<Derived*>(this)->observe(t, s);
        s = static_cast<Derived*>(this)->drive(t, s);

        LOG4CXX_TRACE(m_logger, "@" << t << " out: " << s);
        to_frame(s, this_out);
      }

    protected:
      /** Convert from sim::Runset::Module_frame to type Socket.
       *
       * @param frame Module frame to convert. */
      Socket from_frame(sim::Runset::Module_frame frame) const {
        Socket rv;
        std::copy_n(frame->data(), sizeof(Socket), reinterpret_cast<uint8_t*>(&rv));
        return rv;
      }

      /** Convert to sim::Runset::Module_frame from type Socket.
       *
       * @param socket Source of conversion.
       * @param frame Target of conversion. */
      void to_frame(Socket socket, sim::Runset::Module_frame frame) const {
        std::copy_n(reinterpret_cast<uint8_t*>(&socket), sizeof(Socket), frame->data());
      }


      log4cxx::LoggerPtr m_logger;
  };

}

/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
