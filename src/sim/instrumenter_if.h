#pragma once

#include "ir/time.h"
#include "sim/module_inspector.h"

namespace sim {

  class Instrumenter_if {
    public:
      virtual void push_hierarchy() = 0;
      virtual void pop_hierarchy() = 0;
      virtual void register_module(std::shared_ptr<Module_inspector> insp) = 0;
      virtual void initial(ir::Time const& t) = 0;
      virtual void step(ir::Time const& t) = 0;
  };

}
