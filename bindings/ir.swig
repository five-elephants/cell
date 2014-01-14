%module ir
%include "std_string.i"
%include "std_map.i"
%include "ir/namespace.h"
%{
#include "bindings/api.h"
#include "ir/namespace.h"
%}

/*
%extend ir::Namespace {
	const char* __str__() {
		static char buf[] = "Namespace";
		return buf;
	}
};
*/

namespace std {
    template<class T>
    class shared_ptr{
    public:
        T* operator-> () const;
    };
}

%template(shared_ptr_Module) std::shared_ptr<ir::Module>;

void hello();
ir::Namespace analyze(std::string const& filenmae);
ir::Namespace load(std::string const& filename);
void save(std::string const& filename, ir::Namespace& ns);
