#pragma once

#include "sim/instrumenter_if.h"

#include <ostream>

namespace sim {

  class Stream_instrumenter : public Instrumenter_if {
    public:
      Stream_instrumenter(std::ostream& os) 
        : m_os(os) {
      }

      virtual void push_hierarchy();
      virtual void pop_hierarchy();
      virtual void register_module(std::shared_ptr<Module_inspector> insp);
      virtual void initial(ir::Time const& t);
      virtual void step(ir::Time const& t);

    protected:
      std::ostream& m_os;
  };

}

