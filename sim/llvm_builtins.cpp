#include "sim/llvm_builtins.h"


#define OP_LAMBDA \
      [](llvm::IRBuilder<> bld, llvm::Value* left, llvm::Value* right) -> llvm::Value*  

static void add_operator(ir::Label const& name,
    ir::Label const& return_type_name,
    ir::Label const& left_name,
    ir::Label const& right_name,
    std::function<llvm::Value* (llvm::IRBuilder<>, llvm::Value* left, llvm::Value* right)> insert_func) {
  auto& builtin_types = ir::Builtins<sim::Llvm_impl>::types;

  auto op = std::make_shared<sim::Llvm_operator>();
  op->name = name;
  op->return_type = builtin_types.at(return_type_name);
  op->left = builtin_types.at(left_name);
  op->right = builtin_types.at(right_name);
  op->impl.insert_func = insert_func;
    
  ir::Builtins<sim::Llvm_impl>::operators.insert(std::make_pair(name, op));
}


void init_builtins(std::shared_ptr<sim::Llvm_library> lib) {
  auto& context = lib->impl.context;
  auto& builtin_types = ir::Builtins<sim::Llvm_impl>::types;

  //
  // builtin types
  //

  builtin_types.at("unit")->impl.type = llvm::StructType::get(context, true);
  builtin_types.at("bool")->impl.type = llvm::Type::getInt1Ty(context);
  builtin_types.at("int")->impl.type = llvm::Type::getInt64Ty(context);
  builtin_types.at("void")->impl.type = llvm::Type::getVoidTy(context);
  builtin_types.at("string")->impl.type = llvm::TypeBuilder<char*, false>::get(context);


  //
  // add operators
  //

  add_operator("==", "bool", "int", "int", OP_LAMBDA {
        return bld.CreateICmpEQ(left, right, "cmp_op_equal");
      });
  add_operator("+", "int", "int", "int", OP_LAMBDA {
        return bld.CreateAdd(left, right, "add");
      });
  add_operator("-", "int", "int", "int", OP_LAMBDA {
        return bld.CreateSub(left, right, "sub");
      });
  add_operator("*", "int", "int", "int", OP_LAMBDA {
        return bld.CreateMul(left, right, "mul");
      });
  add_operator("/", "int", "int", "int", OP_LAMBDA {
        return bld.CreateSDiv(left, right, "sdiv");
      });
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
