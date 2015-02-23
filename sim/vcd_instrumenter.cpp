#include "vcd_instrumenter.h"

#include <bitset>


namespace sim {

  Vcd_instrumenter::Vcd_instrumenter(std::string const& filename)
    : m_filename(filename) {
    m_os.open(filename);
    if( !m_os ) {
      std::stringstream strm;
      strm << "Could not open VCD dump file '"
        << filename
        << "' for writing";
      throw std::runtime_error(strm.str());
    }

    write_vcd_header(m_os);
  }


  void
  Vcd_instrumenter::module(ir::Time const& t, Module_inspector& insp) {
    if( m_initial ) {
      write_signal_list(m_os, insp);
      m_initial = false;
    } else {
      write_update(m_os, t, insp);
    }
  }


  void
  Vcd_instrumenter::write_vcd_header(std::ostream& os) {
    os << "$date today $end\n";
    os << "$version CELL's VCD instrumenter v0.0 $end\n";
    os << "$comment $end\n";
    os << "$timescale 1ps $end\n";
  }


  void
  Vcd_instrumenter::write_signal_list(std::ostream& os, Module_inspector& insp) {
    os << "$scope module "
      << insp.module()->name
      << " $end\n";

    for(std::size_t i=0; i<insp.num_elements(); ++i) {
      os << "$var integer 64 "
        << reference(i)
        << ' '
        << insp.get_name(i)
        << " $end\n";
    }

    os << "$upscope $end\n";
    os << "$enddefinitions\n";
    os << "$dumpvars\n";

    for(std::size_t i=0; i<insp.num_elements(); ++i) {
      os << 'b'
        << std::bitset<64>(insp.get<int64_t>(i))
        << ' '
        << reference(i)
        << '\n';
    }

    os << "$end\n";

  }


  void
  Vcd_instrumenter::write_update(std::ostream& os,
      ir::Time const& t,
      Module_inspector& insp) {
    os << '#' << t.v * 1000 << '\n';

    for(std::size_t i=0; i<insp.num_elements(); ++i) {
      os << 'b'
        << std::bitset<64>(insp.get<int64_t>(i))
        << ' '
        << reference(i)
        << '\n';
    }
  }

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */