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
  }


  void
  Vcd_instrumenter::module(ir::Time const& t, Module_inspector& insp) {
    if( m_initial ) {
      write_vcd_header(m_os, t);
      write_signal_list(m_os, insp);
      m_initial = false;
    } else {
      write_update(m_os, t, insp);
    }
  }


  void
  Vcd_instrumenter::write_vcd_header(std::ostream& os, ir::Time const& t) {
    os << "$date today $end\n";
    os << "$version CELL's VCD instrumenter v0.0 $end\n";
    os << "$comment $end\n";
    os << "$timescale ";

    switch(t.magnitude) {
      case ir::Time::s:
        os << "1 s";
        break;

      case ir::Time::ms:
        os << "1 ms";
        break;

      case ir::Time::us:
        os << "1 us";
        break;

      case ir::Time::ns:
        os << "1 ns";
        break;

      case ir::Time::ps:
        os << "1 ps";
        break;

      default:
        throw std::runtime_error("Unsupported timescale");
    }

    os << " $end\n";
  }


  void
  Vcd_instrumenter::write_signal_list(std::ostream& os, Module_inspector& insp) {
    os << "$scope module "
      << insp.module()->name
      << " $end\n";

    for(std::size_t i=0; i<insp.num_elements(); ++i) {
      auto bits = insp.get_bits(i);

      os << "$var "
        << "integer "
        << bits.size()
        << ' '
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
        << insp.get_bits(i)
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
