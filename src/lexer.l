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

std::string g_str;
int first_time = 1 ;
int parsing_error = 0;

std::stack<std::string> fileNames;
std::stack<int> lineNo;
std::vector<std::string> libPaths;

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
                    for( auto libpath : libPaths ) {
                        yyin = fopen( (libpath + fileName).c_str() , "r" );
                        if( yyin )
                            break;
                    }
                    if ( ! yyin ) {
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


<normal>"\n"     { yycolumn = 1; }
<<EOF>>          {
                   if(lineNo.size() > 1 ) {
                       yypop_buffer_state();
                       fileNames.pop();
                       yylineno = lineNo.top();
                       lineNo.pop();
                   } else {
                        yyterminate();
                   }
                 }


\r                      /* cr are ignored */
"if"                    return TOKEN(TIF);
"else"                  return TOKEN(TELSE);
"not"                   return TOKEN(TNOT);
"and"                   return TOKEN(TAND);
"or"                    return TOKEN(TOR);
"func"                  return TOKEN(TFUNDEF);
"var"                   return TOKEN(TVAR);
"val"                   return TOKEN(TVAL);
"while"                 return TOKEN(TWHILE);
"do"                    return TOKEN(TDO);
"for"                   return TOKEN(TFOR);
"in"                    return TOKEN(TIN);
"until"                 return TOKEN(TUNTIL);
"to"                    return TOKEN(TTO);
"step"                  return TOKEN(TSTEP);
"return"                return TOKEN(TRETURN);
"break"                 return TOKEN(TBREAK);
"rm"                    return TOKEN(TFREE);
"true"                  SAVE_BOOLEAN; return TBOOL;
"false"                 SAVE_BOOLEAN; return TBOOL;
\/\/.*                     /* comments one line til nl */
[ \t\n]                 /* ignore */;
\@\@[0-9]+              SAVE_NCHAR; return TCHAR;
\@.                     SAVE_CHAR; return TCHAR;
[a-zA-Z][a-zA-Z0-9_]*   SAVE_TOKEN; return TIDENTIFIER;
[0-9]+                  SAVE_INTEGER; return TINTEGER;
{number}                SAVE_NUMBER; return TDOUBLE;
"="                     return '=';
"=="                    return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"{"                     return '{';
"}"                     return '}';
"("                     return '(';
")"                     return ')';
","                     return ',';
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
"++"                    return TOKEN(TINC);
"--"                    return TOKEN(TDEC);
":"                     return ':';
"?"                     return '?';
"["                     return '[';
"]"                     return ']';
"(."                    return TOKEN(TJOINO);
".)"                    return TOKEN(TJOINC);
.                       {printf("%s: line %d, len %d Unknown token %s !\n", fileNames.top(), yylineno, yyleng, yytext); parsing_error = 1; yyterminate();}

%%
