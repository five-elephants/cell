#pragma once

#include "sim/instrumenter_if.h"

#include <ostream>

namespace sim {

  class Stream_instrumenter : public Instrumenter_if {
    public:
      Stream_instrumenter(std::ostream& os) 
        : m_os(os) {
      }

      virtual void module(ir::Time const& t, Module_inspector& insp);

    protected:
      std::ostream& m_os;
  };

}

