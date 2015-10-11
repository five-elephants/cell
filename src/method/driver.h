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

        Socket s;
        std::copy_n(this_in->data(), sizeof(Socket), reinterpret_cast<uint8_t*>(&s));
        LOG4CXX_INFO(m_logger, "in : " << s);

        static_cast<Base*>(this)->observe(s);
        s = static_cast<Base*>(this)->drive(s);

        LOG4CXX_INFO(m_logger, "out: " << s);
        std::copy_n(reinterpret_cast<uint8_t*>(&s), sizeof(Socket), this_out->data());
      }

    private:
      log4cxx::LoggerPtr m_logger;
  };

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
