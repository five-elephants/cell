#pragma once

#include "ir/codeblock.h"
#include <memory>

namespace ir {

  class Codegen_if {
    public:
      virtual std::shared_ptr<Codeblock_if> make_codeblock(Namespace const& ns) = 0;
      virtual void emit() = 0;
  };

  class Codegen_base : public Codegen_if {
    public:
      Codegen_base();
      virtual ~Codegen_base();
  };

  class Null_codegen : public Codegen_base {
    public:
      Null_codegen();

      virtual std::shared_ptr<Codeblock_if> make_codeblock(Namespace const& ns);
      virtual void emit();
  };

}

