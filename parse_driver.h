#pragma once

#include "parser.tab.h"
#include "ast/node_if.h"
#include "ir/namespace.h"
#include <string>

// Tell Flex the lexer's prototype ...
# define YY_DECL                                        \
  yy::Parser::token_type                         \
  yylex (yy::Parser::semantic_type* yylval,      \
         yy::Parser::location_type* yylloc,      \
         Parse_driver& driver)

// ... and declare it for the parser's sake.
YY_DECL;


class Parse_driver {
  public:
    Parse_driver();
    ~Parse_driver();

    void scan_begin();
    void scan_end();
    int parse(const std::string& f);
    void error(const yy::location& loc, const std::string& m);
    void error(const std::string& m);

    std::string const& filename() const { return m_filename; }
    std::string& filename() { return m_filename;  }

    void ast_root(ast::Node_if& root_node) { m_ast_root = &root_node; }
    ast::Node_if& ast_root() { return *m_ast_root; }

    //ir::Namespace& cur_ns() { return m_default_namespace; }

  private:
    bool m_trace_parsing;
    bool m_trace_scanning;
    std::string m_filename;
    ast::Node_if* m_ast_root;
    //ir::Namespace m_default_namespace;
};
