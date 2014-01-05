#include "ir/analyze.h"

namespace ir {

  Namespace analyze(Node_if const& ast_root, std::string const& defaultname) {
    Namespace rv(defaultname);

    rv.scan_ast(ast_root);
    return rv;
  }

}
