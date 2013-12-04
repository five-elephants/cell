%skeleton "lalr1.cc"
%require "2.5" 
%defines
%define parser_class_name "Parser"

%code requires {
  #include <string>
  #include <vector>
  class Parse_driver;
  namespace ast {
    class Node_if;
    class Unit;
    class Module_def;
    class Function_def;

    typedef std::vector<Node_if*> Node_list;
  }
}

%parse-param { Parse_driver& driver }
%lex-param { Parse_driver& driver }

%locations
%initial-action
{
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &driver.filename();
};

%debug
%error-verbose

%union {
  int ival;
  std::string* sval;
  ast::Unit* unit;
  ast::Node_if* node;
  ast::Module_def* mod;
  ast::Function_def* func;
  ast::Node_list* list;
};

%code {
  #include "parse_driver.h"
  #include "ast/ast.h"
}

%token        END        0            "end of file"
%token        CMP_EQ                  "=="
%token        CMP_NEQ                 "!="
%token        CMP_GT_EQ               ">="
%token        CMP_LT_EQ               "<="
%token        ASSIGN                  "="
%token        CURL_OPEN               "{"
%token        CURL_CLOSE              "}"
%token        PAREN_OPEN              "("
%token        PAREN_CLOSE             ")"
%token        QUOTE                   "\""
%token        VAR                     "var"
%token        DEF                     "def"
%token        MOD                     "mod"
%token        IF                      "if"
%token        ELSE                    "else"
%token        WHILE                   "while"
%token        COLON                   ":"
%token        COMMA                   ","
%token        RETURN                  "return"
%token <sval> IDENTIFIER              "identifier"
%token <ival> NUMBER                  "number"
%token <sval> BITSTRING               "bitstring"

%type <unit> unit
%type <node> unit_item
%type <mod> module
%type <list> module_body
%type <node> module_item
%type <func> func
%type <list> func_params
%type <list> func_params_
%type <node> func_param
%type <node> var_def
%type <list> statements
%type <node> statement
%type <node> func_call
%type <list> exp_list
%type <list> exp_list_
%type <node> exp
%type <node> identifier
%type <node> compound
%type <node> if_statement
%type <node> while_statement
%type <node> return_statement


%printer { debug_stream() << *$$; } "identifier"
%destructor { delete $$; } "identifier"

%printer { debug_stream() << $$; } <ival>



%%

%start unit;

%left '<' '>';
%left '+' '-';
%left '*' '/';

unit: unit unit_item             { $$ = $1; $1->append(*$2); }
    |                            { $$ = new ast::Unit(); driver.ast_root(*$$);};
unit_item: module                { $$ = $1; };
module: "mod" identifier "=" CURL_OPEN module_body CURL_CLOSE
                                 { 
                                  $$ = new ast::Module_def(*$2);
                                  $$->append(*$5);
                                  driver.cur_ns().insert_module(*$$);
                                 };
module_body: module_body module_item
                                 { $$ = $1; $1->push_back($2); }
  |                              { $$ = new ast::Node_list; };
module_item: var_def             { $$ = $1; }
  | func                         { $$ = $1; };

func: "def" identifier "(" func_params ")" ":" identifier "=" statement
                                 { $$ = new ast::Function_def(*$2, *$7); 
                                   $$->append_body(*$9);
                                   $$->append_parameter(*$4);
                                   $$->location(@$); }
func: "def" identifier "(" func_params ")" "=" statement
                                 { $$ = new ast::Function_def(*$2); 
                                   $$->append_body(*$7);
                                   $$->append_parameter(*$4); 
                                   $$->location(@$);}
func_params: func_params_        { $$ = $1; }
  | func_params_ COMMA           { $$ = $1; };
func_params_:                    { $$ = new ast::Node_list; }
  | func_params_ COMMA func_param
                                 { $$ = $1; $1->push_back($3); }
  | func_param                   { $$ = new ast::Node_list; $$->push_back($1); };
func_param: identifier "=" exp ":" identifier
                                 { $$ = new ast::Function_param(*$1, *$5, *$3); }
  | identifier ":" identifier    { auto v = new ast::Function_param(*$1); v->type(*$3); $$ = v; }
  | identifier "=" exp           { auto v = new ast::Function_param(*$1); v->expression(*$3); $$ = v; };

var_def: "var" identifier "=" exp ":" identifier  
                                 { 
                                  $$ = new ast::Variable_def(*$2, *$6, *$4);
                                  $$->location(@$);
                                 }
  | "var" identifier ":" identifier
                                 { auto v = new ast::Variable_def(*$2); v->type(*$4); $$ = v; $$->location(@$);}
  | "var" identifier "=" exp     { auto v = new ast::Variable_def(*$2); v->expression(*$4); $$ = v; $$->location(@$);}
  | "var" identifier             { auto v = new ast::Variable_def(*$2); $$ = v; $$->location(@$);}

statements: statements statement { $$ = $1; $1->push_back($2); }
  |                              { $$ = new ast::Node_list; };
statement: if_statement          { $$ = $1; }
  | while_statement              { $$ = $1; }
  | var_def                      { $$ = $1; }
  | compound                     { $$ = $1; }
  | func_call                    { $$ = $1; }
  | return_statement             { $$ = $1; };
compound: CURL_OPEN statements CURL_CLOSE
                                 { auto v = new ast::Compound(); v->statements(*$2); $$ = v; };

if_statement: "if" PAREN_OPEN exp PAREN_CLOSE statement 
                                 { $$ = new ast::If_statement(*$3, *$5); }
  | "if" PAREN_OPEN exp PAREN_CLOSE statement "else" statement
                                 { auto v = new ast::If_statement(*$3, *$5); v->else_body(*$7); $$ = v; };
while_statement: "while" PAREN_OPEN exp PAREN_CLOSE statement
                                 { $$ = new ast::While_statement(*$3, *$5); };
func_call: identifier PAREN_OPEN exp_list PAREN_CLOSE
                                 { auto v = new ast::Function_call(*$1); v->expressions(*$3); $$ = v; }
  | identifier PAREN_OPEN PAREN_CLOSE
                                 { auto v = new ast::Function_call(*$1); $$ = v; };
return_statement: "return" exp   { auto v = new ast::Statement("return_statement", {$2}); $$ = v; };
exp_list: exp_list_              { $$ = $1; }
  | exp_list_ COMMA              { $$ = $1; };
exp_list_: exp_list_ COMMA exp   { $$ = $1; $1->push_back($3); }
  | exp                          { $$ = new ast::Node_list; $$->push_back($1); };
exp: exp '+' exp                 { $$ = new ast::Op_plus(*$1, *$3); }
  | exp '-' exp                  { $$ = new ast::Op_minus(*$1, *$3); }
  | exp '*' exp                  { $$ = new ast::Op_mult(*$1, *$3); }
  | exp '/' exp                  { $$ = new ast::Op_div(*$1, *$3); }
  | exp '<' exp                  { $$ = new ast::Op_lesser_then(*$1, *$3); }
  | exp '>' exp                  { $$ = new ast::Op_greater_then(*$1, *$3); }
  | exp "==" exp                 { $$ = new ast::Op_equal(*$1, *$3); }
  | exp "!=" exp                 { $$ = new ast::Op_not_equal(*$1, *$3); }
  | exp ">=" exp                 { $$ = new ast::Op_greater_or_equal_then(*$1, *$3); }
  | exp "<=" exp                 { $$ = new ast::Op_lesser_or_equal_then(*$1, *$3); }
  | identifier                   { $$ = $1; }
  | "number"                     { $$ = new ast::Literal<int>($1); }
  | "bitstring"                  { $$ = new ast::Bitstring_literal(*$1); }
  | func_call                    { $$ = $1; };
identifier: "identifier"         { $$ = new ast::Identifier(*$1); delete $1; $$->location(@$); };

%%



void
yy::Parser::error(yy::Parser::location_type const& loc, std::string const& msg) {
  driver.error(loc, msg);
}


/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
