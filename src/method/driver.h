#pragma once

#include "logging/logger.h"
#include "sim/simulation_engine.h"

namespace method {

  template<typename Base, typename Socket>
  class Driver {
    public:
      Driver()
        : m_logger(log4cxx::Logger::getLogger("tb.Driver")) {
      }

      void operator () (ir::Time const& t,
          sim::Runset::Module_frame this_in,
          sim::Runset::Module_frame this_out,
          sim::Runset::Module_frame this_prev) {

        Socket s = from_frame(this_in);
        LOG4CXX_TRACE(m_logger, "@" << t << " in : " << s);

        static_cast<Base*>(this)->observe(t, s);
        s = static_cast<Base*>(this)->drive(t, s);

        LOG4CXX_TRACE(m_logger, "@" << t << " out: " << s);
        to_frame(s, this_out);
      }

    protected:
      Socket from_frame(sim::Runset::Module_frame frame) const {
        Socket rv;
        std::copy_n(frame->data(), sizeof(Socket), reinterpret_cast<uint8_t*>(&rv));
        return rv;
      }

      void to_frame(Socket socket, sim::Runset::Module_frame frame) const {
        std::copy_n(reinterpret_cast<uint8_t*>(&socket), sizeof(Socket), frame->data());
      }


      log4cxx::LoggerPtr m_logger;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
