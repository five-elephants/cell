#pragma once

#include "namespace.h"

namespace ir {
  inline bool type_compatible(Type const& a, Type const& b) {
    return (a.name == b.name);
  }
}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
