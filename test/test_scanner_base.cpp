#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#include "ast/ast.h"
#include "ast/scanner_base.h"


std::shared_ptr<ast::Node_if> generate_test_tree() {
  using namespace ast;

  Namespace_def* root;
  Identifier* root_id = new Identifier("root");
  Identifier* func_id = new Identifier("function");
  Identifier* func_ret_type_id = new Identifier("int");

  root = new Namespace_def(*root_id);
  
  Function_def* func_def = new Function_def(*func_id, *func_ret_type_id);
  root->append(*func_def);

  return std::shared_ptr<Node_if>(root);
}


class My_test_scanner : public ast::Scanner_base<My_test_scanner> {
  public:
    bool enter_namespace_def(ast::Node_if const& node) {
      std::cout << "entering a namespace" << std::endl;
      return true;
    }

    My_test_scanner() {
      enter_if_type<ast::Namespace_def>([](My_test_scanner&, ast::Node_if const& node) -> bool {
        std::cout << "hui" << std::endl;
        return true;
      });
      enter_if_type<ast::Namespace_def>(&My_test_scanner::enter_namespace_def);
    }
};



TEST(ast, scanner_base) {
  auto tree = generate_test_tree();
  My_test_scanner scanner;

  tree->accept(scanner);
}
