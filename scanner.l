%{
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <string>
#include <sstream>
#include "parse_driver.h"
#include "parser.tab.h"

/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
	 not conform to C89.  See Debian bug 333231
	 <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
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
	typedef yy::Parser::token token;
%}

"mod"       return token::MOD;
"var"       return token::VAR;
[-+*/]      return yy::Parser::token_type(yytext[0]);
"="         return token::ASSIGN;
"{"         return token::CURL_OPEN;
"}"         return token::CURL_CLOSE;
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
Parse_driver::scan_begin() {
	yy_flex_debug = m_trace_scanning;
	if( m_filename == "-" )
		yyin = stdin;
	else if( !(yyin = fopen(m_filename.c_str(), "r")) ) {
		error(std::string("cannot open ") + m_filename);
		exit(1);
	}
}

void
Parse_driver::scan_end() {
	fclose(yyin);
}

