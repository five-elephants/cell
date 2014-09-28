#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "ast/scanner_chain.h"


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

class My_test_scanner;


bool do_no_enter() {
  std::cout << "do_no_enter" << std::endl;
  return false;
}


bool free_enter_namespace_def(My_test_scanner& scanner, ast::Namespace_def const& node) {
  std::cout << "entering namespace from free function." << std::endl;
  return true;
} 


bool free_enter_namespace_def_node(ast::Namespace_def const& node) {
  return true;
}


bool free_enter_namespace_def_noargs() {
  return true;
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
      on_enter_if_type<ast::Namespace_def>(free_enter_namespace_def);
      on_enter_if_type<ast::Namespace_def>(&free_enter_namespace_def);
      on_enter_if_type<ast::Namespace_def>(free_enter_namespace_def_noargs);
      on_enter_if_type<ast::Namespace_def>(free_enter_namespace_def_node);
      on_enter(&My_test_scanner::member_do_no_enter);
      on_enter(do_no_enter);
      on_enter([]() -> bool { std::cout << "hello" << std::endl; return true; });
      //std::function<bool()> f( []() -> bool { return false; } );
      //do_always(m_enter_callbacks, f);
    }
};



TEST(ast, scanner_base) {
  auto tree = generate_test_tree();
  My_test_scanner scanner;

  tree->accept(scanner);
}


TEST(ast, scanner_chaining) {
  auto tree = generate_test_tree();
  My_test_scanner scanner;
  //ast::Ast_printer printer;
  My_test_scanner printer;
  
  ast::Scanner_chain chain(printer, scanner);

  tree->accept(chain);
}

