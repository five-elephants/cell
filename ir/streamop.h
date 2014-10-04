#pragma once

#include <ostream>
#include "namespace.h"


template<typename Impl>
std::ostream& operator << (std::ostream& os, ir::Type<Impl> const& type) {
  return os << type.name;
}

