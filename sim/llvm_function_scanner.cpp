#include "llvm_function_scanner.h"


namespace sim {

  Llvm_function_scanner::Llvm_function_scanner(Llvm_namespace& ns, Llvm_function& function)
    : m_ns(ns),
      m_function(function) {

  }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
