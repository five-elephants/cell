%skeleton "lalr1.cc"
%require "2.5" 
%defines
%define parser_class_name "Parser"

%code requires {
	#include <string>
	class Parse_driver;
	namespace ast {
		class Node_if;
		class Module_def;
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
};

%code {
	#include "parse_driver.h"
	#include "ast/ast.h"
}

%token        END        0            "end of file"
%token        ASSIGN                  "="
%token        CURL_OPEN               "{"
%token        CURL_CLOSE              "}"
%token        VAR                     "var"
%token        MOD                     "mod"
%token        COLON                   ":"
%token <sval> IDENTIFIER              "identifier"
%token <ival> NUMBER                  "number"

%type <mod> module
%type <node> module_item
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

unit: module { driver.ast_root(*$1); };
module: module "}" { $$ = $1; }
	| module module_item { $$ = $1; $1->append(*$2); }
	| "mod" identifier "=" "{" { $$ = new ast::Module_def(*$2); };
module_item: exp  { $$ = $1; };
exp: exp '+' exp   { $$ = new ast::Op_plus(*$1, *$3); }
	| exp '-' exp    { $$ = new ast::Op_minus(*$1, *$3); }
	| exp '*' exp    { $$ = new ast::Op_mult(*$1, *$3); }
	| exp '/' exp    { $$ = new ast::Op_div(*$1, *$3); }
	| identifier     { $$ = $1; }
	| "number"       { $$ = new ast::Literal<int>($1); };
identifier: "identifier"  { $$ = new ast::Identifier(*$1); delete $1; };

%%



void
yy::Parser::error(yy::Parser::location_type const& loc, std::string const& msg) {
  driver.error(loc, msg);
}

