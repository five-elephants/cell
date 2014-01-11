#include "streamop.h"

std::ostream& operator << (std::ostream& os, ir::Type const& type) {
  return os << type.name;
}
