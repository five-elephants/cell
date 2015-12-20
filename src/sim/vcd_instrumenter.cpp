#include "vcd_instrumenter.h"

#include <bitset>


namespace sim {

  // local helper functions
  static std::string reference(std::size_t i);
  static std::size_t write_value(std::ostream& os,
      std::shared_ptr<Module_inspector> const& insp,
      std::size_t index,
      std::size_t ref);



  Vcd_instrumenter::Vcd_instrumenter(std::string const& filename)
    : m_filename(filename),
      m_logger(log4cxx::Logger::getLogger("cell.vcd")) {
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
    auto mod = insp->module();
    os << "$scope module "
      << mod->name
      << " $end\n";

    for(std::size_t i=0; i<insp->num_elements(); ++i) {
      auto obj = insp->get_object(i);
      // Don't include pointers to instantiated modules
      if( mod->instantiations.count(obj->name) != 0 )
        continue;

      if( obj->type == ir::Builtins<sim::Llvm_impl>::types["float"] ) {
        os << "$var "
          << "real 1 "
          << reference(m_ref_counter++)
          << ' '
          << insp->get_name(i)
          << " $end\n";
      } else if( !obj->type->elements.empty() ) {
        auto bits = insp->get_element_bits(i);
        std::size_t j = 0;
        for(auto const& elem : obj->type->elements) {
          if( elem.second->type == ir::Builtins<sim::Llvm_impl>::types["float"] )
            os << "$var "
              << "real 1 "
              << reference(m_ref_counter++)
              << ' '
              << insp->get_name(i)
              << '.'
              << elem.first
              << " $end\n";
          else
            os << "$var "
              << "integer "
              << bits[j].size()
              << ' '
              << reference(m_ref_counter++)
              << ' '
              << insp->get_name(i)
              << '.'
              << elem.first
              << " $end\n";
          ++j;
        }
      } else {
        auto bits = insp->get_bits(i);

        if( bits.size() > 0 ) {
          os << "$var "
            << "integer "
            << bits.size()
            << ' '
            << reference(m_ref_counter++)
            << ' '
            << insp->get_name(i)
            << " $end\n";
        }
      }
    }
  }

  void
  Vcd_instrumenter::write_dump_all(std::ostream& os) {
    os << "$dumpvars\n";

    std::size_t ref_counter = 0;
    for(auto const& insp : m_inspected) {
      for(std::size_t i=0; i<insp->num_elements(); ++i)
        ref_counter = write_value(os, insp, i, ref_counter);
    }

    os << "$end\n";
  }


  void
  Vcd_instrumenter::write_update(std::ostream& os,
      ir::Time const& t) {
    os << '#' << t.value(m_unit) << '\n';

    std::size_t ref_counter = 0;
    for(auto const& insp : m_inspected) {
      for(std::size_t i=0; i<insp->num_elements(); ++i)
        ref_counter = write_value(os, insp, i, ref_counter);
    }
  }



  std::string reference(std::size_t i) {
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



  static std::size_t write_value(std::ostream& os,
      std::shared_ptr<Module_inspector> const& insp,
      std::size_t index,
      std::size_t ref) {
    auto mod = insp->module();
    auto obj = insp->get_object(index);

    if( mod->instantiations.count(obj->name) != 0 )
      return ref;

    if( obj->type == ir::Builtins<sim::Llvm_impl>::types["float"] ) {
      os << 'r'
        << insp->get<double>(index)
        << ' '
        << reference(ref++)
        << '\n';
    } else if( !obj->type->elements.empty() ) {
      auto bits = insp->get_element_bits(index);

      for(auto const& b : bits) {
        os << 'b'
          << b
          << ' '
          << reference(ref++)
          << '\n';
      }
    } else {
      auto bits = insp->get_bits(index);
      if( bits.size() > 0 ) {
        os << 'b'
          << bits
          << ' '
          << reference(ref++)
          << '\n';
      }
    }

    return ref;
  }
}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
