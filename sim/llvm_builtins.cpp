#include "sim/llvm_builtins.h"


#define OP_LAMBDA \
      [&builtin_types](llvm::IRBuilder<> bld, llvm::Value* left, llvm::Value* right) -> llvm::Value*
#define OP_C_LAMBDA \
      [&builtin_types](llvm::Constant* left, llvm::Constant* right) -> llvm::Constant*

static void add_operator(ir::Label const& name,
    ir::Label const& return_type_name,
    ir::Label const& left_name,
    ir::Label const& right_name,
    std::function<llvm::Value* (llvm::IRBuilder<>, llvm::Value* left, llvm::Value* right)> insert_func,
    std::function<llvm::Constant* (llvm::Constant*, llvm::Constant*)> const_insert_func = nullptr) {
  auto& builtin_types = ir::Builtins<sim::Llvm_impl>::types;

  auto op = std::make_shared<sim::Llvm_operator>();
  op->name = name;
  op->return_type = builtin_types.at(return_type_name);
  op->left = builtin_types.at(left_name);
  op->right = builtin_types.at(right_name);
  op->impl.insert_func = insert_func;
  op->impl.const_insert_func = const_insert_func;

  ir::Builtins<sim::Llvm_impl>::operators.insert(std::make_pair(name, op));
}


static void add_function(ir::Label const& name,
    std::shared_ptr<sim::Llvm_library> lib) {
  std::vector<llvm::Type*> args;
  auto& builtin_funcs = ir::Builtins<sim::Llvm_impl>::functions;
  auto f = builtin_funcs.at(name);
  auto module = lib->impl.module.get();

  for(auto p : f->parameters)
    args.push_back(p->type->impl.type);

  auto ty = llvm::FunctionType::get(f->return_type->impl.type,
      args,
      false);
  f->impl.func_type = ty;
  f->impl.code = llvm::Function::Create(ty,
      llvm::Function::ExternalLinkage,
      name,
      module
      );
}


void init_builtins(std::shared_ptr<sim::Llvm_library> lib) {
  auto& context = lib->impl.context;
  auto module = lib->impl.module.get();
  auto& builtin_types = ir::Builtins<sim::Llvm_impl>::types;
  auto& builtin_funcs = ir::Builtins<sim::Llvm_impl>::functions;


  ir::Builtins<sim::Llvm_impl>::init();

  //
  // builtin types
  //

  builtin_types.at("unit")->impl.type = llvm::StructType::get(context, true);
  builtin_types.at("bool")->impl.type = llvm::Type::getInt1Ty(context);
  builtin_types.at("int")->impl.type = llvm::Type::getInt64Ty(context);
  builtin_types.at("void")->impl.type = llvm::Type::getVoidTy(context);
  builtin_types.at("string")->impl.type = llvm::TypeBuilder<char*, false>::get(context);
  builtin_types.at("float")->impl.type = llvm::Type::getDoubleTy(context);


  //
  // add operators
  // (ordered by operator and data type)
  //

  add_operator("==", "bool", "int", "int", OP_LAMBDA {
        return bld.CreateICmpEQ(left, right, "cmp_op_equal");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_EQ,
          left,
          right);
      });
  add_operator("==", "bool", "float", "float", OP_LAMBDA {
        return bld.CreateFCmpUEQ(left, right, "fcmp_op_equal");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getCompare(llvm::CmpInst::FCMP_UEQ,
          left,
          right);
      });
  add_operator("!=", "bool", "int", "int", OP_LAMBDA {
        return bld.CreateICmpEQ(left, right, "cmp_op_not_equal");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getCompare(llvm::CmpInst::ICMP_NE,
          left,
          right);
      });
  add_operator("+", "int", "int", "int", OP_LAMBDA {
        return bld.CreateAdd(left, right, "add");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getAdd(left, right);
      });
  add_operator("+", "float", "float", "float", OP_LAMBDA {
        return bld.CreateFAdd(left, right, "fadd");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getFAdd(left, right);
      });
  add_operator("-", "int", "int", "int", OP_LAMBDA {
        return bld.CreateSub(left, right, "sub");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getSub(left, right);
      });
  add_operator("-", "float", "float", "float", OP_LAMBDA {
        return bld.CreateFSub(left, right, "fsub");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getFSub(left, right);
      });
  add_operator("*", "int", "int", "int", OP_LAMBDA {
        return bld.CreateMul(left, right, "mul");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getMul(left, right);
      });
  add_operator("*", "float", "float", "float", OP_LAMBDA {
        return bld.CreateFMul(left, right, "fmul");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getFMul(left, right);
      });
  add_operator("/", "int", "int", "int", OP_LAMBDA {
        return bld.CreateSDiv(left, right, "sdiv");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getSDiv(left, right);
      });
  add_operator("/", "float", "float", "float", OP_LAMBDA {
        return bld.CreateFDiv(left, right, "fdiv");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getFDiv(left, right);
      });
  add_operator("%", "int", "int", "int", OP_LAMBDA {
        return bld.CreateSRem(left, right, "rem");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getSRem(left, right);
      });
  add_operator("!", "bool", "bool", "bool", OP_LAMBDA {
        return bld.CreateNot(left, "not");
      });
  add_operator("@", "bool", "bool", "bool", OP_LAMBDA {
        return bld.CreateICmpNE(left, right, "cmp_at");
      });
  add_operator("@", "bool", "int", "int", OP_LAMBDA {
        return bld.CreateICmpNE(left, right, "cmp_at");
      });
  add_operator("@", "bool", "float", "float", OP_LAMBDA {
        return bld.CreateFCmpONE(left, right, "cmp_at");
      });
  add_operator("&&", "bool", "bool", "bool", OP_LAMBDA {
        return bld.CreateAnd(left, right, "and");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getAnd(left, right);
      });
  add_operator("||", "bool", "bool", "bool", OP_LAMBDA {
        return bld.CreateAnd(left, right, "or");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getOr(left, right);
      });
  add_operator(">", "bool", "float", "float", OP_LAMBDA {
        return bld.CreateFCmpUGT(left, right, "fcmp_gt");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getFCmp(llvm::CmpInst::FCMP_UGT, left, right);
      });
  add_operator(">", "bool", "int", "int", OP_LAMBDA {
        return bld.CreateICmpSGT(left, right, "cmp_gt");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_SGT, left, right);
      });
  add_operator("<", "bool", "float", "float", OP_LAMBDA {
        return bld.CreateFCmpULT(left, right, "fcmp_lt");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getFCmp(llvm::CmpInst::FCMP_ULT, left, right);
      });
  add_operator("<", "bool", "int", "int", OP_LAMBDA {
        return bld.CreateICmpSLT(left, right, "cmp_lt");
      }, OP_C_LAMBDA {
        return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_SLT, left, right);
      });

  // type conversion operators
  //   to float
  add_operator("convert", "float", "int", "int", OP_LAMBDA {
        return bld.CreateSIToFP(left,
          builtin_types["float"]->impl.type,
          "int_to_float");
      });
  //   to int
  add_operator("convert", "int", "float", "float", OP_LAMBDA {
        return bld.CreateFPToSI(left,
          builtin_types["int"]->impl.type,
          "float_to_int");
      });


  //
  // builtin functions
  //

  add_function("print", lib);
  add_function("rand", lib);
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
