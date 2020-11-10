%code requires {

# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */

}

%{
    #include "ast.h"
    //#include "Conditional.h"
    //#include "CompareOperator.h"
    #include "variable.h"
    #include "binaryop.h"
    #include "unaryop.h"
    #include "return.h"
    #include "assignment.h"
    #include "function.h"
    //#include "WhileLoop.h"
    //#include "Array.h"
    //#include "Range.h"

    #include <stdio.h>
    #include <stack>
    mlang::Block *programBlock; /* the top level root node of our final AST */

    extern int yylex();
    int yyerror(char const * s );
    #define YYERROR_VERBOSE
    #define YYDEBUG 1

    extern std::stack<std::string> fileNames;

    # define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
          (Current).file_name = fileNames.top();            \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
          (Current).file_name = fileNames.top();            \
        }                                                               \
    while (0)

%}

/* Represents the many different ways we can access our data */
%union {
    mlang::Node *node;
    mlang::Block *block;
    mlang::Expression *expr;
    mlang::Statement *stmt;
    mlang::Identifier *ident;
    mlang::VariableDeclaration *var_decl;
    std::vector<mlang::VariableDeclaration*> *varvec;
    std::vector<mlang::Expression*> *exprvec;
    std::string *string;
    long long integer;
    double number;
    int boolean;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TSTR
%token <integer> TINTEGER
%token <number> TDOUBLE
%token <boolean> TBOOL
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE
%token <token> TLTLT "<<"
%token <token> TRANGE
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TNOT TAND TOR
%token <token> TINC TDEC
%token <token> TIF TELSE TWHILE
%token <token> TFUNDEF TRETURN TVAR TVAL
/*%token <token> INDENT UNINDENT*/

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (Identifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> literals expr boolean_expr binop_expr unaryop_expr array_expr array_access  range_expr
%type <varvec> func_decl_args
%type <exprvec> call_args array_elemets_expr
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl func_arg_decl conditional return while array_add_element
%type <token> comparison

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV
%left TAND TNOT
%right TINC TDEC

%start program
%debug
%verbose
%locations /* track locations: @n of component N; @$ of entire range */
/*
%define parse.lac full
%define lr.type ielr
*/

%%

program : %empty { programBlock = new mlang::Block(); }
        | stmts { programBlock = $1; }
        ;

stmts : stmt { $$ = new mlang::Block(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : var_decl
     | func_decl
     | conditional
     | return
     | while
     | array_add_element
     | expr { $$ = new mlang::ExpressionStatement($1); }
     ;

block : '{' stmts '}' { $$ = $2; }
      | '{' '}' { $$ = new mlang::Block(); }
      ;

conditional : TIF expr block TELSE block {/*$$ = new mlang::Conditional($2,$3,$5);*/}
            | TIF expr block {/*$$ = new mlang::Conditional($2,$3);*/}
            ;

while : TWHILE '(' expr ')' block TELSE block {/*$$ = new mlang::WhileLoop($3,$5,$7);*/}
      | TWHILE '(' expr ')' block {/*$$ = new mlang::WhileLoop($3,$5);*/}
      ;

/*var_decl : ident ident { $$ = new mlang::VariableDeclaration($1, $2, @$); }
         | ident ident '=' expr { $$ = new mlang::VariableDeclaration($1, $2, $4, @$); }
         | TVAR ident { $$ = new mlang::VariableDeclaration($2, "var", @$); }
         | TVAR ident '=' expr { $$ = new mlang::VariableDeclaration($2, "var", $4, @$); }
         | TVAL ident { $$ = new mlang::VariableDeclaration($2, "val", @$); }
         | TVAL ident '=' expr { $$ = new mlang::VariableDeclaration($2, "val", $4, @$); }
         ;*/

var_decl : TVAR ident '=' expr { $$ = new mlang::VariableDeclaration($2, "var", $4, @$); }
         | TVAL ident '=' expr { $$ = new mlang::VariableDeclaration($2, "val", $4, @$); }
         ;

func_decl : TFUNDEF ident '(' func_decl_args ')' ':' ident block { $$ = new mlang::FunctionDeclaration($7, $2, $4, $8, @$); }
          | TFUNDEF ident '(' func_decl_args ')' block { $$ = new mlang::FunctionDeclaration($2, $4, $6, @$); }
          ;

func_decl_args : %empty  { $$ = new mlang::VariableList(); }
          | func_arg_decl { $$ = new mlang::VariableList(); $$->push_back($<var_decl>1); }
          | func_decl_args ',' func_arg_decl { $1->push_back($<var_decl>3); }
          ;

func_arg_decl : ident ident { $$ = new mlang::VariableDeclaration($1, $2, @$); }
            | ident ident '=' expr { $$ = new mlang::VariableDeclaration($1, $2, $4, @$); }
            ;

return : TRETURN { $$ = new mlang::Return(@$); }
       | TRETURN expr { $$ = new mlang::Return(@$, $2); }
       ;

expr : ident '=' expr { $$ = new mlang::Assignment($<ident>1, $3, @$); }
     | ident '(' call_args ')' { $$ = new mlang::FunctionCall($1, $3, @$);  }
     | ident { $<ident>$ = $1; }
     | literals
     | boolean_expr
     | binop_expr
     | unaryop_expr
     | '(' expr ')' { $$ = $2; }
     | range_expr
     | array_expr
     | array_access
     ;

ident : TIDENTIFIER { $$ = new mlang::Identifier(*$1, @1); delete $1; }
      ;

literals : TINTEGER { $$ = new mlang::Integer($1); }
         | TDOUBLE { $$ = new mlang::Double($1); }
         | TSTR { $$ = new mlang::String(*$1); delete $1; }
         | TBOOL { $$ = new mlang::Boolean($1); }
         ;

/* have to write it explicit to have the right operator precedence */
binop_expr : expr TAND expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | expr TOR expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | expr TPLUS expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | expr TMINUS expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | expr TMUL expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | expr TDIV expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           ;

unaryop_expr : TNOT expr { $$ = new mlang::UnaryOp($1, $2, @$); }
             | TMINUS expr { $$ = new mlang::UnaryOp($1, $2, @$); }
             | TPLUS expr { $$ = new mlang::UnaryOp($1, $2, @$); }
             | expr TINC { $$ = new mlang::UnaryOp($2, $1, @$); }
             | expr TDEC { $$ = new mlang::UnaryOp($2, $1, @$); }
             ;

boolean_expr : expr comparison expr { /*$$ = new mlang::CompOperator($1, $2, $3);*/ }
             ;

call_args : %empty  { $$ = new mlang::ExpressionList(); }
          | expr { $$ = new mlang::ExpressionList(); $$->push_back($1); }
          | call_args ',' expr  { $1->push_back($3); }
          ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
           ;

array_elemets_expr: %empty {$$ = new mlang::ExpressionList(); }
                 | expr {$$ = new mlang::ExpressionList(); $$->push_back($1);}
                 | array_elemets_expr ',' expr {$$->push_back($3); }
                 ;

array_expr : '[' array_elemets_expr ']' {/*$$ = new mlang::Array($2, @$);*/}
          ;

array_add_element: ident "<<" expr { /*$$ = new mlang::ArrayAddElement($1, $3, @$);*/ }
                ;

array_access: ident '[' TINTEGER ']' { /*$$ = new mlang::ArrayAccess($1, $3, @$);*/ }
           | array_access '[' TINTEGER ']' { /*$$ = new mlang::ArrayAccess($1, $3, @$);*/ }
           ;

range_expr : '[' expr TRANGE expr ']' {/*$$ = new mlang::Range($2, $4, @$);*/}
           ;

%%
