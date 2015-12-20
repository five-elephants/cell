#pragma once

#include "ir/namespace.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
//#include <llvm/PassManager.h>
//#include <llvm/Analysis/Passes.h>
//#include <llvm/Analysis/Verifier.h>
//#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/TypeBuilder.h>
#include <stdexcept>


/** Simulation related namespace */
namespace sim {

  struct Socket_operator_codegen;


  struct Llvm_impl {
    struct Type {
      llvm::Type* type;
      std::shared_ptr<Socket_operator_codegen> opgen_left;
      std::shared_ptr<Socket_operator_codegen> opgen_right;
    };

    struct Port {
      std::size_t struct_index;
    };

    struct Object {
      std::size_t struct_index;
    };

    struct Constant {
      llvm::Constant* expr = nullptr;
    };

    struct Port_assignment {};
    struct Instantiation {};

    struct Function {
      llvm::Function* code;
      llvm::FunctionType* func_type;
    };

    struct Operator {
      std::function<llvm::Value* (llvm::IRBuilder<>, llvm::Value* left, llvm::Value* right)> insert_func;
      std::function<llvm::Constant* (llvm::Constant* left, llvm::Constant* right)> const_insert_func;
    };

    struct Process {};
    struct Periodic {};
    struct Once {};
    struct Recurrent {};
    struct Socket {};
    struct Namespace {};

    struct Module {
      llvm::StructType* mod_type;
      llvm::Function* ctor;
    };

    struct Module_template {};

    struct Library {
      llvm::LLVMContext& context;
      std::unique_ptr<llvm::IRBuilder<>> builder;
      std::unique_ptr<llvm::Module> module;
      //std::unique_ptr<llvm::FunctionPassManager> fpm;


      Library()
        : context(llvm::getGlobalContext()) {
      }

      Library(Library&& other)
        : context(other.context) {
        builder = std::move(other.builder);
        module = std::move(other.module);
        //fpm = std::move(other.fpm);
      }

      Library& operator = (Library&& o) {
        if( &context != &o.context )
          throw std::runtime_error("can not move assign! different LLVMContext objects");

        builder = std::move(o.builder);
        module = std::move(o.module);
        //fpm = std::move(o.fpm);

        return *this;
      }

    };
  };


  typedef ir::Type<Llvm_impl> Llvm_type;
  typedef ir::Port<Llvm_impl> Llvm_port;
  typedef ir::Object<Llvm_impl> Llvm_object;
  typedef ir::Constant<Llvm_impl> Llvm_constant;
  typedef ir::Port_assignment<Llvm_impl> Llvm_port_assignment;
  typedef ir::Instantiation<Llvm_impl> Llvm_instantiation;
  typedef ir::Function<Llvm_impl> Llvm_function;
  typedef ir::Operator<Llvm_impl> Llvm_operator;
  typedef ir::Process<Llvm_impl> Llvm_process;
  typedef ir::Periodic<Llvm_impl> Llvm_periodic;
  typedef ir::Once<Llvm_impl> Llvm_once;
  typedef ir::Recurrent<Llvm_impl> Llvm_recurrent;
  typedef ir::Socket<Llvm_impl> Llvm_socket;
  typedef ir::Namespace<Llvm_impl> Llvm_namespace;
  typedef ir::Module<Llvm_impl> Llvm_module;
  typedef ir::Library<Llvm_impl> Llvm_library;



  //
  // constructor functions
  //

  extern Llvm_impl::Library create_library_impl(std::string const& name);

}


/* vim: set et ff=unix sts=0 sw=2 ts=2 : */
