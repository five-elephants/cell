%{
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <string>
#include <sstream>
#include "calc++-driver.h"
#include "calc++-parser.tab.h"

#undef yywrap
#define yywrap() 1

#define yyterminate() return token::END
%}


%option noyywrap nounput batch debug

id     [a-zA-Z][a-zA-Z_0-9]*
int    [0-9]+
blank  [ \t]

%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%

%{
	yylloc->step();
%}

{blank}+     yylloc->step();
[\n]+        yylloc->lines(yyleng); yylloc->step();

%{
	typedef yy::calcxx_parser::token token;
%}

"var"       return token::VAR;
[-+*/]      return yy::calcxx_parser::token_type(yytext[0]);
"="         return token::ASSIGN;
":"         return token::COLON;
{int}       {
	std::stringstream strm(yytext);
	strm >> yylval->ival;
	return token::NUMBER;
}
{id}       yylval->sval = new std::string(yytext); return token::IDENTIFIER;
.          driver.error(*yylloc, "invalid character");

%%


void
calcxx_driver::scan_begin() {
	yy_flex_debug = trace_scanning;
	if( file == "-" )
		yyin = stdin;
	else if( !(yyin = fopen(file.c_str(), "r")) ) {
		error(std::string("cannot open ") + file);
		exit(1);
	}
}

void
calcxx_driver::scan_end() {
	fclose(yyin);
}

