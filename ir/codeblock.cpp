#include "ir/codeblock.h"

namespace ir {

  Codeblock_base::~Codeblock_base() {
  }

  void
  Null_codeblock::scan_ast(ast::Node_if const& tree) {
  }

  void
  Null_codeblock::append_predefined_objects(std::map<Label, std::shared_ptr<Object>> objects) {
  }

  void
  Null_codeblock::prototype(std::shared_ptr<Function> func) {
  }

  void
  Null_codeblock::enclosing_module(std::shared_ptr<Module> mod) {
  }
}

