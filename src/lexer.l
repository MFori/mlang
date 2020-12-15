%{
#include <string>
#include <stack>
#include "ast.h"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define SAVE_INTEGER yylval.integer = std::stoll(std::string(yytext, yyleng))
#define SAVE_NUMBER yylval.number = std::stod(std::string(yytext, yyleng))
#define SAVE_BOOLEAN yylval.boolean = std::string(yytext, yyleng) == "true" ? 1 : 0
#define SAVE_CHAR yylval.character = yytext[1]
#define SAVE_NCHAR yylval.character = (char) std::stoi(std::string(yytext + 2, yyleng-2))
#define TOKEN(t) (yylval.token = t)

#ifdef _MSC_VER
int isatty(int) {return 0;};
#endif

extern "C" int yywrap() { return 1; }

int yycolumn = 1;

#define YYERROR_VERBOSE 1
int yyerror(char const * s )
{
    printf("ERROR %s in '%s' at line %d col %d\n", s, yytext, yylineno, yycolumn);
    printf("  parsed %s %d.%d-%d.%d\n", yylloc.file_name.c_str(), yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
    return 1;
}

int open_brackets = 0;
int last_token = -1;
#define RET(token) last_token = token; return token

int need_semicolon() {
     if(open_brackets > 0) {
        return 0;
     }
     return last_token == TIDENTIFIER ||
     	last_token == TINTEGER ||
     	last_token == TCHAR ||
     	last_token == TDOUBLE ||
     	last_token == TBOOL ||
     	last_token == TRETURN ||
     	last_token == TBREAK ||
     	last_token == TFREE ||
     	last_token == TINC ||
     	last_token == TDEC ||
     	last_token == TJOINC ||
     	last_token == ')';
}

std::string g_str;
int first_time = 1 ;
int parsing_error = 0;

std::stack<std::string> fileNames;
std::stack<int> lineNo;

#define YY_USER_ACTION do { \
    if( yylloc.last_line < yylineno ) yycolumn = 1 ; \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + (int)yyleng - 1; \
    yycolumn += (int)yyleng; \
    yylloc.file_name = fileNames.top(); \
    } while(0) ;

%}

%option yylineno

%s normal
%x str qstr
%x comment
%x incl

dig     [0-9]
num1    {dig}+\.([eE][-+]?{dig}+)?
num2    {dig}*\.{dig}+([eE][-+]?{dig}+)?
number  {num1}|{num2}

%%
                    if( first_time ) {
                          first_time = 0;
                          lineNo.push(yylineno);
                          BEGIN normal;
                    }

import              BEGIN(incl);
<incl>[ \t]*        /* eat the whitespace */
<incl>[^ \t\n\r]+   { /* got the include file name */
                    std::string fileName = yytext;
                    std::size_t pos = fileName.find(".mlang");
                    if( pos == std::string::npos ) {
                        fileName += ".mlang";
                    }
                    yyin = fopen(fileName.c_str(), "r");
                    if (! yyin) {
                       printf( "%s in %s line %d\n", (std::string("Failed to load import file ") + fileName).c_str(), fileNames.top().c_str(), yylineno );
                       parsing_error = 1;
                       yyterminate();
                    } else {
                       fileNames.push(yytext);
                       lineNo.push(yylineno);
                       yylineno = yycolumn = 1;
                       yylloc.first_line = yylloc.first_column =  yylloc.last_line = yylloc.last_column = 1;
                       yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
                    }
                    BEGIN(normal);
                    }


\/\*                BEGIN(comment);
<comment>\*\/       BEGIN(normal);
<comment>.          ;/* eat everything */
<comment>\n         ;/* eat everything */

\"      g_str = ""; BEGIN(str);
'       g_str = ""; BEGIN(qstr);

<str>\" {   BEGIN(normal);
            yylval.string = new std::string(g_str);
            return TSTR;
        }
<qstr>' {   BEGIN(normal);
            yylval.string = new std::string(g_str);
            return TSTR;
        }

<str,qstr>\\n   g_str += "\n";
<str,qstr>\\t   g_str += "\t";
<str,qstr>\\r   g_str += "\r";
<str>\\\"       g_str += "\"";
<qstr>\\\'      g_str += "'";

<str,qstr>\\(.|\n)  g_str += yytext[1];

<str>[^\\\"]+  g_str += std::string(yytext);
<qstr>[^\\']+  g_str += std::string(yytext);


<<EOF>>          {
                   if(lineNo.size() > 1 ) {
                       yypop_buffer_state();
                       fileNames.pop();
                       yylineno = lineNo.top();
                       lineNo.pop();
                       if(need_semicolon()) { RET(';'); }
                   } else {
                       if(need_semicolon()) { RET(';'); }
                       yyterminate();
                   }
                 }


\r                      /* cr are ignored */
"if"                    RET(TOKEN(TIF));
"else"                  RET(TOKEN(TELSE));
"not"                   RET(TOKEN(TNOT));
"and"                   RET(TOKEN(TAND));
"or"                    RET(TOKEN(TOR));
"func"                  RET(TOKEN(TFUNDEF));
"var"                   RET(TOKEN(TVAR));
"val"                   RET(TOKEN(TVAL));
"while"                 RET(TOKEN(TWHILE));
"do"                    RET(TOKEN(TDO));
"for"                   RET(TOKEN(TFOR));
"in"                    RET(TOKEN(TIN));
"until"                 RET(TOKEN(TUNTIL));
"to"                    RET(TOKEN(TTO));
"step"                  RET(TOKEN(TSTEP));
"return"                RET(TOKEN(TRETURN));
"break"                 RET(TOKEN(TBREAK));
"rm"                    RET(TOKEN(TFREE));
"true"                  SAVE_BOOLEAN; RET(TBOOL);
"false"                 SAVE_BOOLEAN; RET(TBOOL);
\/\/.*                  /* comments one line til nl */
[ \t]                   /* ignore */;
\@\@[0-9]+              SAVE_NCHAR; RET(TCHAR);
\@.                     SAVE_CHAR; RET(TCHAR);
[a-zA-Z][a-zA-Z0-9_]*   SAVE_TOKEN; RET(TIDENTIFIER);
[0-9]+                  SAVE_INTEGER; RET(TINTEGER);
{number}                SAVE_NUMBER; RET(TDOUBLE);
"="                     RET('=');
"=="                    RET(TOKEN(TCEQ));
"!="                    RET(TOKEN(TCNE));
"<"                     RET(TOKEN(TCLT));
"<="                    RET(TOKEN(TCLE));
">"                     RET(TOKEN(TCGT));
">="                    RET(TOKEN(TCGE));
"{"                     RET('{');
"}"                     RET('}');
"("                     { open_brackets++; RET('('); }
")"                     { open_brackets--; RET(')'); }
","                     RET(',');
"+"                     RET(TOKEN(TPLUS));
"-"                     RET(TOKEN(TMINUS));
"*"                     RET(TOKEN(TMUL));
"/"                     RET(TOKEN(TDIV));
"++"                    RET(TOKEN(TINC));
"--"                    RET(TOKEN(TDEC));
":"                     RET(':');
"?"                     RET('?');
"["                     RET('[');
"]"                     RET(']');
"(."                    RET(TOKEN(TJOINO));
".)"                    RET(TOKEN(TJOINC));
\n                      { if(need_semicolon()) { RET(';'); }}
";"                     RET(';');
.                       {printf("%s: line %d, len %d Unknown token %s !\n", fileNames.top(), yylineno, yyleng, yytext); parsing_error = 1; yyterminate();}

%%
