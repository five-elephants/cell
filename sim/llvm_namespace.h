#pragma once

#include "ir/namespace.h"


namespace sim {

	struct Llvm_type : public ir::Type {
    Llvm_type() : Type() {}

    Llvm_type(ir::Label _name)
      : Type(_name) {
    }
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
    Llvm_socket() 
      : Socket() {
    }
	};


	struct Llvm_namespace : public ir::Namespace {
    Llvm_namespace() 
      : Namespace() {
    }

    Llvm_namespace(ir::Label const& _name)
      : Namespace(_name) {
    }
	};


	struct Llvm_module : public ir::Module {
    Llvm_module() 
      : Module() {
    }

    Llvm_module(ir::Label const& label)
      : Module(label) {
    }
	};

}


/* vim: set noet fenc= ff=unix sts=0 sw=2 ts=2 : */
