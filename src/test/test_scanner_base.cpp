#include <gtest/gtest.h>
#include <memory>
#include <iostream>

#include "parse_driver.h"
#include "ast/ast.h"
#include "ast/scanner_base.h"
#include "ast/scanner_chain.h"
#include "ast/ast_printer.h"


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

    virtual bool virtual_member(ast::Namespace_def const& node) {
      std::cout << "virtual_member" << std::endl;
      return true;
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

      on_enter_if_type<ast::Namespace_def>(&My_test_scanner::virtual_member);
    }
};


class My_derived_test_scanner : public My_test_scanner {
  public:
    bool m_derived_ran = false;

    My_derived_test_scanner()
      : My_test_scanner() {
    }
    

    virtual bool virtual_member(ast::Namespace_def const& node) {
      m_derived_ran = true;
      std::cout << "derived virtual_member" << std::endl;
      return true;
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
  ast::Ast_printer printer(std::cout);
  
  ast::Scanner_chain chain(printer, scanner);

  tree->accept(chain);
}


TEST(ast, ast_printer) {
  Parse_driver parser;
  ast::Ast_printer printer(std::cout);

  EXPECT_EQ(parser.parse("lib/test/basic_process.cell"), 0);
  parser.ast_root().accept(printer);
}



TEST(ast, scanner_virtual_derived) {
  auto tree = generate_test_tree();
  My_derived_test_scanner scanner;

  tree->accept(scanner);

  EXPECT_EQ(scanner.m_derived_ran, true);
}

