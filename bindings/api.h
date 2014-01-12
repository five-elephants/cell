#pragma once

#include "ir/namespace.h"

#include <string>

extern void hello();
extern ir::Namespace load(std::string const& filename);
extern void save(std::string const& filename, ir::Namespace& ns);
