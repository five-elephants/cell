#include "ast/function_param.h"

namespace ast {

  Function_param::Function_param(Node_if& identifier)
    : Variable_def(identifier) {
  }

  Function_param::Function_param(Node_if& identifier,
      Node_if& type,
      Node_if& expression)
    : Variable_def(identifier, type, expression) {
  }

  Function_param::~Function_param() {
  }

}
