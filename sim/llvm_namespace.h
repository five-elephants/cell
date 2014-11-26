#pragma once

#include "ir/namespace.h"


namespace sim {
	
  struct Llvm_impl {
    struct Type {};
    struct Port {};
    struct Object {};
    struct Port_assignment {};
    struct Instantiation {};
    struct Function {};
    struct Process {};
    struct Periodic {};
    struct Socket {};
    struct Namespace {};
    struct Module {};
    struct Library {};
  };


	typedef ir::Type<Llvm_impl> Llvm_type;
	typedef ir::Port<Llvm_impl> Llvm_port;
	typedef ir::Object<Llvm_impl> Llvm_object;
	typedef ir::Port_assignment<Llvm_impl> Llvm_port_assignment;
	typedef ir::Instantiation<Llvm_impl> Llvm_instantiation;
	typedef ir::Function<Llvm_impl> Llvm_function;
	typedef ir::Process<Llvm_impl> Llvm_process;
	typedef ir::Periodic<Llvm_impl> Llvm_periodic;
	typedef ir::Socket<Llvm_impl> Llvm_socket;
	typedef ir::Namespace<Llvm_impl> Llvm_namespace;
	typedef ir::Module<Llvm_impl> Llvm_module;
	typedef ir::Library<Llvm_impl> Llvm_library;

}


/* vim: set noet fenc= ff=unix sts=0 sw=2 ts=2 : */
