%{
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <string>
#include <sstream>
#include "parse_driver.h"
#include "parser.tab.hh"

/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
	 not conform to C89.  See Debian bug 333231
	 <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
#undef yywrap
#define yywrap() 1

#define yyterminate() return token::END
%}


%option noyywrap nounput batch debug

id         [a-zA-Z_][a-zA-Z_0-9]*
int        [0-9]+
bitstring  \"[01_-]*\"
blank      [ \t]

%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%x IN_COMMENT

%%

%{
	yylloc->step();
%}

{blank}+     yylloc->step();
[\n]+        yylloc->lines(yyleng); yylloc->step();

<INITIAL>{
"/*"         BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"         BEGIN(INITIAL);
[^*\n]+
"*"
\n           yylineno++;
}

"//".*

%{
	typedef yy::Parser::token token;
%}

"socket"    return token::SOCKET;
"namespace" return token::NAMESPACE;
"mod"       return token::MOD;
"def"       return token::DEF;
"var"       return token::VAR;
"if"        return token::IF;
"else"      return token::ELSE;
"while"     return token::WHILE;
"return"    return token::RETURN;
[-+*/<>]    return yy::Parser::token_type(yytext[0]);
"="         return token::ASSIGN;
"=="        return token::CMP_EQ;
"!="        return token::CMP_NEQ;
">="        return token::CMP_GT_EQ;
"<="        return token::CMP_LT_EQ;
"=>"        return token::DARROW_RIGHT;
"<>"        return token::BIDIR;
"{"         return token::CURL_OPEN;
"}"         return token::CURL_CLOSE;
"("         return token::PAREN_OPEN;
")"         return token::PAREN_CLOSE;
"\""        return token::QUOTE;
":"         return token::COLON;
","         return token::COMMA;
"in"        return token::IN;
"out"       return token::OUT;
"bidir"     return token::BIDIR2;
"process"   return token::PROCESS;
"periodic"  return token::PERIODIC;
{int}       {
	std::stringstream strm(yytext);
	strm >> yylval->ival;
	return token::NUMBER;
}
{bitstring} {
  std::string str(yytext);
  yylval->sval = new std::string(str.substr(1, str.size()-3));
  return token::BITSTRING;
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

