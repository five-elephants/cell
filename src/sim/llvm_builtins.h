#pragma once

#include "ir/builtins.h"
#include "sim/llvm_namespace.h"

extern void init_builtins(std::shared_ptr<sim::Llvm_library> lib);
