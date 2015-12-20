#pragma once

#include "namespace.h"

namespace ir {
  template<typename Impl>
  bool type_compatible(Type<Impl> const& a, Type<Impl> const& b) {
    return (a.name == b.name);
  } // XXX questionable!
}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
