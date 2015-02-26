#pragma once

#include "sim/instrumenter_if.h"

#include <string>
#include <fstream>


namespace sim {

  class Vcd_instrumenter : public Instrumenter_if {
    public:
      Vcd_instrumenter(std::string const& filename);

      virtual void module(ir::Time const& t, Module_inspector& insp);

    protected:
      std::string m_filename;
      std::ofstream m_os;
      bool m_initial = true;

      std::string reference(std::size_t i) const {
        static char const lowest = 33;
        static char const highest = 126;
        static std::size_t const num_codes = highest - lowest;

        std::string rv;

        rv.reserve(i / num_codes + 1);

        for(std::size_t j = 0; j < (i / num_codes); ++j)
          rv.push_back(highest);

        rv.push_back(lowest + (i % num_codes));

        return rv;
      }

      virtual void write_vcd_header(std::ostream& os, ir::Time const& t);
      virtual void write_signal_list(std::ostream& os, Module_inspector& insp);
      virtual void write_update(std::ostream& os,
          ir::Time const& t,
          Module_inspector& insp);
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
