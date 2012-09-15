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
%token        ASSIGN                  "="
%token        CURL_OPEN               "{"
%token        CURL_CLOSE              "}"
%token        PAREN_OPEN              "("
%token        PAREN_CLOSE             ")"
%token        VAR                     "var"
%token        DEF                     "def"
%token        MOD                     "mod"
%token        COLON                   ":"
%token        COMMA                   ","
%token <sval> IDENTIFIER              "identifier"
%token <ival> NUMBER                  "number"

%type <mod> module
%type <list> module_body
%type <node> module_item
%type <func> func
%type <list> func_params
%type <list> func_params_
%type <node> func_param
%type <list> func_body
%type <node> func_item
%type <node> var_def
%type <node> exp
%type <node> identifier


%printer { debug_stream() << *$$; } "identifier"
%destructor { delete $$; } "identifier"

%printer { debug_stream() << $$; } <ival>



%%

%start unit;

// unit: statements { driver.set_statements($1); };
// 
// statements: statements statement { $$ = $1; $1->push_back($2); }
// 		  | { $$ = new Statement_list(); };
// 
// statement: assignment { $$ = $1; }
// 		 | exp { $$ = $1; };
// 
// assignment:
// 	"var" "identifier" "=" exp ":" "identifier"          { $$ = new Assignment(driver, *$2, $4, $6); delete $2; delete $6; };

%left '+' '-';
%left '*' '/';

unit: module                     { driver.ast_root(*$1); };
module: "mod" identifier "=" CURL_OPEN module_body CURL_CLOSE
			                           { $$ = new ast::Module_def(*$2); $$->append(*$5); };
module_body: module_body module_item
					                       { $$ = $1; $1->push_back($2); }
	|                              { $$ = new ast::Node_list; };
module_item: var_def             { $$ = $1; }
	| func                         { $$ = $1; };

func: "def" identifier "(" func_params ")" ":" identifier "=" CURL_OPEN func_body CURL_CLOSE
		                             { $$ = new ast::Function_def(*$2, *$7); 
																	 $$->append_body(*$10);
		                               $$->append_parameter(*$4); }
func_params: func_params_        { $$ = $1; }
	| func_params_ COMMA           { $$ = $1; };
func_params_: func_params_ COMMA func_param
																 { $$ = $1; $1->push_back($3); }
  | func_param                   { $$ = new ast::Node_list; $$->push_back($1); };
func_param: identifier "=" exp ":" identifier
					                       { $$ = new ast::Variable_def(*$1, *$5, *$3); }
  | identifier ":" identifier    { auto v = new ast::Variable_def(*$1); v->type(*$3); $$ = v; }
  | identifier "=" exp           { auto v = new ast::Variable_def(*$1); v->expression(*$3); $$ = v; };
func_body: func_body func_item   { $$ = $1; $1->push_back($2); }
	|                              { $$ = new ast::Node_list; };
func_item: var_def               { $$ = $1; }; 

var_def: "var" identifier "=" exp ":" identifier  
			                           { $$ = new ast::Variable_def(*$2, *$6, *$4); }
  | "var" identifier ":" identifier
                                 { auto v = new ast::Variable_def(*$2); v->type(*$4); $$ = v; }
  | "var" identifier "=" exp     { auto v = new ast::Variable_def(*$2); v->expression(*$4); $$ = v; }
  | "var" identifier             { auto v = new ast::Variable_def(*$2); $$ = v; }
exp: exp '+' exp                 { $$ = new ast::Op_plus(*$1, *$3); }
	| exp '-' exp                  { $$ = new ast::Op_minus(*$1, *$3); }
	| exp '*' exp                  { $$ = new ast::Op_mult(*$1, *$3); }
	| exp '/' exp                  { $$ = new ast::Op_div(*$1, *$3); }
	| identifier                   { $$ = $1; }
	| "number"                     { $$ = new ast::Literal<int>($1); };
identifier: "identifier"         { $$ = new ast::Identifier(*$1); delete $1; };

%%



void
yy::Parser::error(yy::Parser::location_type const& loc, std::string const& msg) {
  driver.error(loc, msg);
}

