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

      char reference(std::size_t i) const {
        return 33 + i;
      }

      virtual void write_vcd_header(std::ostream& os);
      virtual void write_signal_list(std::ostream& os, Module_inspector& insp);
      virtual void write_update(std::ostream& os,
          ir::Time const& t,
          Module_inspector& insp);
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
