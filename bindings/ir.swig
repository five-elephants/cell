%module ir
%include "std_string.i"
%{
#include "bindings/api.h"
%}

void hello();
ir::Namespace load(std::string const& filename);
void save(std::string const& filename, ir::Namespace& ns);
