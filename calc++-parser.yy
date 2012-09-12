%skeleton "lalr1.cc"
%require "2.5" 
%defines
%define parser_class_name "calcxx_parser"

%code requires {
	#include <string>
	#include <vector>
	class calcxx_driver;
	class Statement;
	class Expression;
	typedef std::vector<Statement*> Statement_list;
}

%parse-param { calcxx_driver& driver }
%lex-param { calcxx_driver& driver }

%locations
%initial-action
{
	// Initialize the initial location.
	@$.begin.filename = @$.end.filename = &driver.file;
};

%debug
%error-verbose

%union {
	int ival;
	std::string* sval;
	Statement_list* stmts;
	Statement* stmt;	
	Expression* exp;
};

%code {
	#include "calc++-driver.h"
	#include "ast.h"
}

%token        END        0            "end of file"
%token        ASSIGN                  "="
%token        VAR                     "var"
%token        COLON                   ":"
%token <sval> IDENTIFIER              "identifier"
%token <ival> NUMBER                  "number"
%type <stmts> unit
%type <stmts> statements
%type <stmt> statement
%type <exp> exp
%type <stmt> assignment


%printer { debug_stream() << *$$; } "identifier"
%destructor { delete $$; } "identifier"

%printer { debug_stream() << $$; } <ival>



%%



%start unit;

unit: statements { driver.set_statements($1); };

statements: statements statement { $$ = $1; $1->push_back($2); }
		  | { $$ = new Statement_list(); };

statement: assignment { $$ = $1; }
		 | exp { $$ = $1; };

assignment:
	"var" "identifier" "=" exp ":" "identifier"          { $$ = new Assignment(driver, *$2, $4, $6); delete $2; delete $6; };

%left '+' '-';
%left '*' '/';

exp: exp '+' exp     { $$ = new Bin_op_exp(Operator::plus, $1, $3); }
	| exp '-' exp    { $$ = new Bin_op_exp(Operator::minus, $1, $3); }
	| exp '*' exp    { $$ = new Bin_op_exp(Operator::mult, $1, $3); }
	| exp '/' exp    { $$ = new Bin_op_exp(Operator::div, $1, $3); }
	| "identifier"   { $$ = new Identifier(driver, *$1); delete $1; }
	| "number"       { $$ = new Number($1); };



%%



void
yy::calcxx_parser::error(yy::calcxx_parser::location_type const& loc, std::string const& msg) {
  driver.error(loc, msg);
}

