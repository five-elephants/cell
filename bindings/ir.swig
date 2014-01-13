%module ir
%include "std_string.i"
%{
#include "bindings/api.h"
%}

void hello();
ir::Namespace analyze(std::string const& filenmae);
ir::Namespace load(std::string const& filename);
void save(std::string const& filename, ir::Namespace& ns);
