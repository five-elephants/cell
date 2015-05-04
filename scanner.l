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
float      [-+]{0,1}[0-9]+\.[0-9]+
bitstring  \'[01_-]*\'
string     \"[[:print:]]*\"
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
"inst"      return token::INST;
"def"       return token::DEF;
"const"     return token::CONST;
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
"&&"        return token::AND;
"||"        return token::OR;
"<>"        return token::BIDIR;
"{"         return token::CURL_OPEN;
"}"         return token::CURL_CLOSE;
"["         return token::SQUARE_OPEN;
"]"         return token::SQUARE_CLOSE;
"("         return token::PAREN_OPEN;
")"         return token::PAREN_CLOSE;
"\""        return token::QUOTE;
"%"         return token::PERCENT;
"!"         return token::BANG;
"@"         return token::AT;
":"         return token::COLON;
"::"        return token::DCOLON;
"->"        return token::ARROW_RIGHT;
"."         return token::POINT;
","         return token::COMMA;
";"         return token::SEMICOLON;
"in"        return token::IN;
"out"       return token::OUT;
"bidir"     return token::BIDIR2;
"process"   return token::PROCESS;
"periodic"  return token::PERIODIC;
"once"      return token::ONCE;
"recurrent" return token::RECURRENT;
"true"      return token::TRUE;
"false"     return token::FALSE;
"template"  return token::TEMPLATE;
{int}       {
	std::stringstream strm(yytext);
	strm >> yylval->ival;
	return token::NUMBER;
}
{float} {
 std::stringstream strm(yytext);
 strm >> yylval->fval;
 return token::FLOAT;
}
{bitstring} {
  std::string str(yytext);
  yylval->sval = new std::string(str.substr(1, str.size()-3));
  return token::BITSTRING;
}
{string} {
  std::string str(yytext);
  yylval->sval = new std::string(str.substr(1, str.size()-2));
  return token::STRING;
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

