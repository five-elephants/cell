#pragma once

#include "ir/namespace.h"

#include <string>

extern void hello();
extern ir::Namespace analyze(std::string const& filenmae);
extern ir::Namespace load(std::string const& filename);
extern void save(std::string const& filename, ir::Namespace& ns);

extern ir::Module find_module(ir::Namespace const& ns, ir::Label const& name);
