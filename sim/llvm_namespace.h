#pragma once

#include "ir/namespace.h"


namespace sim {

	struct Llvm_type : public ir::Type {
	};


	struct Llvm_object : public ir::Object {
	};


	struct Llvm_port_assignment : public ir::Port_assignment {
	};


	struct Llvm_instantiation : public ir::Instantiation {
	};


	struct Llvm_function : public ir::Function {
	};


	struct Llvm_process : public ir::Process {
	};


	struct Llvm_periodic : public ir::Periodic {
	};


	struct Llvm_port : public ir::Port {
	};


	struct Llvm_socket : public ir::Socket {
	};


	struct Llvm_namespace : public ir::Namespace {
	};


	struct Llvm_module : public ir::Module {
	};

}


/* vim: set noet fenc= ff=unix sts=0 sw=2 ts=2 : */
