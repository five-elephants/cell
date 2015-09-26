#pragma once

#include "ast/variable_def.h"

namespace ast {

  class Function_param : public Variable_def {
    public:
      Function_param(Node_if& identifier);
      Function_param(Node_if& identifier,
          Node_if& type,
          Node_if& expression);
  };

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
