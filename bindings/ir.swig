%module ir
%include "std_string.i"
//%include "ir/namespace.h"
%{
#include "bindings/api.h"
//#include "ir/namespace.h"
%}

/*%extend ir::Namespace {
	const char* __str__() {
		static char buf[] = "Namespace";
		return buf;
	}
};*/

void hello();
ir::Namespace analyze(std::string const& filenmae);
ir::Namespace load(std::string const& filename);
void save(std::string const& filename, ir::Namespace& ns);
