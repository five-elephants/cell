#pragma once

#include <llvm/IR/IRBuilder.h>

#include "sim/llvm_namespace.h"

namespace sim {

  struct Socket_operator_codegen {
    std::shared_ptr<Llvm_type> socket;
    ir::Direction direction;

    Socket_operator_codegen(std::shared_ptr<Llvm_type> sock,
        ir::Direction dir)
      : socket(sock),
        direction(dir) {
    }


    llvm::Value* operator () (llvm::IRBuilder<> bld,
        llvm::Value* left,
        llvm::Value* right) {
      for(auto const& elem : socket->elements) {
        auto idx = elem.second->impl.struct_index;
        auto lhs = bld.CreateStructGEP(left, idx, elem.first + "_lhs");
        auto rhs = bld.CreateStructGEP(right, idx, elem.first + "_rhs");
        llvm::Value* tmp;

        if( (elem.second->direction == ir::Direction::Bidirectional)
            || (elem.second->direction == direction) ) {
          tmp = bld.CreateLoad(rhs,
              std::string("load_") + elem.first + "_rhs");
          bld.CreateStore(tmp, lhs);
        }

        if( (elem.second->direction == ir::Direction::Bidirectional)
            || (elem.second->direction == direction) ) {
          tmp = bld.CreateLoad(lhs,
              std::string("load_") + elem.first + "_lhs");
          bld.CreateStore(tmp, rhs);
        }
      }

      return left;
    }
  };

}
