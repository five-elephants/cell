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

bool do_no_enter() {
  std::cout << "do_no_enter" << std::endl;
  return false;
}

class My_test_scanner : public ast::Scanner_base<My_test_scanner> {
  public:
    bool enter_namespace_def(ast::Namespace_def const& node) {
      std::cout << "entering a namespace" << std::endl;
      return true;
    }

    bool member_do_no_enter() {
      std::cout << "member_do_no_enter" << std::endl;
      return false;
    }


    My_test_scanner() {
      on_enter_if_type<ast::Namespace_def>(&My_test_scanner::enter_namespace_def);
      on_enter(&My_test_scanner::member_do_no_enter);
      on_enter(do_no_enter);
      //on_enter([]() -> bool { return false; });
      //std::function<bool()> f( []() -> bool { return false; } );
      //do_always(m_enter_callbacks, f);
    }
};



TEST(ast, scanner_base) {
  auto tree = generate_test_tree();
  My_test_scanner scanner;

  tree->accept(scanner);
}

