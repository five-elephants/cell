#include "ir/codeblock.h"

namespace ir {

  Codeblock_base::~Codeblock_base() {
  }

  void
  Null_codeblock::scan_ast(Namespace& enclosing_ns, ast::Node_if const& tree) {
  }

  void append_predefined_objects(std::map<Label, std::shared_ptr<Object>> objects) {
  }

}

