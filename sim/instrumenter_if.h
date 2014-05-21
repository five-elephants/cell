#pragma once

#include "ir/time.h"
#include "sim/module_inspector.h"

namespace sim {

  class Instrumenter_if {
    public:
      virtual void module(ir::Time const& t, Module_inspector& insp) = 0;
  };

}
