#pragma once

#include "sim/instrumenter_if.h"

#include "logging/logger.h"

#include <string>
#include <fstream>


namespace sim {

  class Vcd_instrumenter : public Instrumenter_if {
    public:
      Vcd_instrumenter(std::string const& filename);

      virtual void push_hierarchy();
      virtual void pop_hierarchy();
      virtual void register_module(std::shared_ptr<Module_inspector> insp);
      virtual void initial(ir::Time const& t);
      virtual void step(ir::Time const& t);


    protected:
      std::string m_filename;
      std::ofstream m_os;
      bool m_initial = true;
      ir::Time::Unit m_unit;
      std::vector<std::shared_ptr<Module_inspector>> m_inspected;
      std::size_t m_ref_counter = 0;
      log4cxx::LoggerPtr m_logger;

      virtual void write_vcd_header(std::ostream& os, ir::Time const& t);
      virtual void write_signal_list(std::ostream& os,
          std::shared_ptr<Module_inspector> insp);
      virtual void write_dump_all(std::ostream& os);
      virtual void write_update(std::ostream& os,
          ir::Time const& t);
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
