#include "vcd_instrumenter.h"

#include <bitset>


namespace sim {

  Vcd_instrumenter::Vcd_instrumenter(std::string const& filename)
    : m_filename(filename),
      m_logger(log4cxx::Logger::getLogger("cell.vcd")) {
    m_logger->setLevel(log4cxx::Level::getTrace());
    m_os.open(filename);
    if( !m_os ) {
      std::stringstream strm;
      strm << "Could not open VCD dump file '"
        << filename
        << "' for writing";
      throw std::runtime_error(strm.str());
    }
    LOG4CXX_INFO(m_logger, "Opened '" << filename << "' for VCD output");

    write_vcd_header(m_os, ir::Time(0, ir::Time::ps));
  }


  void
  Vcd_instrumenter::push_hierarchy() {
  }


  void
  Vcd_instrumenter::pop_hierarchy() {
    m_os << "$upscope $end\n";
  }


  void
  Vcd_instrumenter::register_module(std::shared_ptr<Module_inspector> insp) {
    LOG4CXX_DEBUG(m_logger, "register module '"
        << insp->module()->name
        << "'");

    m_inspected.push_back(insp);
    write_signal_list(m_os, insp);
  }


  void
  Vcd_instrumenter::initial(ir::Time const& t) {
    LOG4CXX_TRACE(m_logger, "initial dump");
    m_os << "$enddefinitions\n";

    write_dump_all(m_os);
  }


  void
  Vcd_instrumenter::step(ir::Time const& t) {
    LOG4CXX_TRACE(m_logger, "step to time " << t);
    write_update(m_os, t);
  }


  void
  Vcd_instrumenter::write_vcd_header(std::ostream& os, ir::Time const& t) {
    LOG4CXX_TRACE(m_logger, "writing VCD header");

    os << "$date today $end\n";
    os << "$version CELL's VCD instrumenter v0.0 $end\n";
    os << "$comment $end\n";
    os << "$timescale ";

    m_unit = (ir::Time::Unit)t.magnitude;

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
  Vcd_instrumenter::write_signal_list(std::ostream& os,
      std::shared_ptr<Module_inspector> insp) {
    os << "$scope module "
      << insp->module()->name
      << " $end\n";

    for(std::size_t i=0; i<insp->num_elements(); ++i) {
      auto obj = insp->get_object(i);

      if( obj->type == ir::Builtins<sim::Llvm_impl>::types["float"] ) {
        os << "$var "
          << "real 1 "
          << reference(m_ref_counter + i)
          << ' '
          << insp->get_name(i)
          << " $end\n";
      } else {
        auto bits = insp->get_bits(i);

        os << "$var "
          << "integer "
          << bits.size()
          << ' '
          << reference(m_ref_counter + i)
          << ' '
          << insp->get_name(i)
          << " $end\n";
      }
    }

    m_ref_counter += insp->num_elements();
  }

  void
  Vcd_instrumenter::write_dump_all(std::ostream& os) {
    os << "$dumpvars\n";

    std::size_t ref_counter = 0;
    for(auto const& insp : m_inspected) {
      for(std::size_t i=0; i<insp->num_elements(); ++i) {
        auto obj = insp->get_object(i);

        if( obj->type == ir::Builtins<sim::Llvm_impl>::types["float"] ) {
          os << 'r'
            << insp->get<double>(i)
            << ' '
            << reference(ref_counter + i)
            << '\n';
        } else {
          os << 'b'
            << insp->get_bits(i)
            << ' '
            << reference(ref_counter + i)
            << '\n';
        }
      }

      ref_counter += insp->num_elements();
    }

    os << "$end\n";
  }


  void
  Vcd_instrumenter::write_update(std::ostream& os,
      ir::Time const& t) {
    os << '#' << t.value(m_unit) << '\n';

    std::size_t ref_counter = 0;
    for(auto const& insp : m_inspected) {
      for(std::size_t i=0; i<insp->num_elements(); ++i) {
        auto obj = insp->get_object(i);
        if( obj->type == ir::Builtins<sim::Llvm_impl>::types["float"] ) {
          os << 'r'
            << insp->get<double>(i)
            << ' '
            << reference(ref_counter + i)
            << '\n';
        } else {
          os << 'b'
            << std::bitset<64>(insp->get<int64_t>(i))
            << ' '
            << reference(ref_counter + i)
            << '\n';
        }
      }

      ref_counter += insp->num_elements();
    }
  }

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
