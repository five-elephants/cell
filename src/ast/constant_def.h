#pragma once

#include "variable_def.h"


namespace ast {

  class Constant_def : public Variable_def {
    public:
      Constant_def(Node_if& identifier)
        : Variable_def(identifier) {
      }


      Constant_def(Node_if& identifier,
          Node_if& type,
          Node_if& expression)
        : Variable_def(identifier, type, expression) {
      }

  };

}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
