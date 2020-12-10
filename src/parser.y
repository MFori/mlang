%code requires {

# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */

}

%{
    #include "ast.h"
    #include "str.h"
    #include "variable.h"
    #include "assignment.h"
    #include "unaryop.h"
    #include "binaryop.h"
    #include "ternaryop.h"
    #include "return.h"
    #include "break.h"
    #include "free.h"
    #include "function.h"
    #include "conditional.h"
    #include "comparison.h"
    #include "whileloop.h"
    #include "forloop.h"
    #include "foreach.h"
    #include "range.h"
    #include "array.h"
    #include "cast.h"

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
    mlang::Range *range;
    mlang::VariableDeclaration *var_decl;
    std::vector<mlang::VariableDeclaration*> *varvec;
    std::vector<mlang::Expression*> *exprvec;
    std::string *string;
    long long integer;
    double number;
    int boolean;
    char character;
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
%token <character> TCHAR
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE
%token <token> TPLUS TMINUS TMUL TDIV TJOINO TJOINC
%token <token> TNOT TAND TOR
%token <token> TINC TDEC
%token <token> TIF TELSE TWHILE TDO TFOR TIN TUNTIL TTO TSTEP
%token <token> TFUNDEF TRETURN TBREAK TFREE TVAR TVAL

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (Identifier*). It makes the compiler happy.
 */
%type <expr> primary_expr expr postfix_expr assignment_expr unary_expr ternary_expr compare_expr or_expr and_expr binop_expr literals
%type <ident> ident
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt lstmt expression_statement var_decl func_decl func_arg_decl conditional return break free while for
%type <range> range

/* Operator precedence for mathematical operators */
%left TINTEGER
%left TPLUS TMINUS
%left TMUL TDIV
%left TAND TOR TNOT
%left TCEQ
%left TCNE
%left TCLT
%left TCLE
%left TCGT
%left TCGE
%left TINC TDEC

%start program
%debug
%verbose
%locations /* track locations: @n of component N; @$ of entire range */

%%

program : %empty { programBlock = new mlang::Block(); }
        | stmts { programBlock = $1; }
        ;

stmts : stmt { $$ = new mlang::Block(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : expression_statement
     | var_decl ';'
     | func_decl
     | conditional
     | return ';'
     | break ';'
     | free ';'
     | while
     | for
     ;

lstmt : expr { $$ = new mlang::ExpressionStatement($1); }
      | var_decl
      | return
      | break
      | free
      ;

expression_statement : expr ';' { $$ = new mlang::ExpressionStatement($1); }
                     | ';' { $$ = 0; }
                     ;

block : '{' stmts '}' { $$ = $2; }
      | '{' stmts lstmt '}' { $$ = $2; $2->statements.push_back($<stmt>3); }
      | '{' '}' { $$ = new mlang::Block(); }
      ;

primary_expr : ident { $<ident>$ = $1; }
             | ident '(' call_args ')' { $$ = new mlang::FunctionCall($1, $3, @$);  }
             | '(' expr ')' { $$ = $2; }
             | literals
             | TJOINO call_args TJOINC { $$ = new mlang::StringJoin($2, @$); }
             ;

postfix_expr : primary_expr
             | postfix_expr '[' expr ']' { $$ = new mlang::ArrayAccess($1, $3, @$); }
             | postfix_expr TINC { $$ = new mlang::UnaryOp($2, $1, 0, @$); }
             | postfix_expr TDEC { $$ = new mlang::UnaryOp($2, $1, 0, @$); }
             ;

unary_expr : postfix_expr
           | TINC unary_expr { $$ = new mlang::UnaryOp($1, 0, $2, @$); }
           | TDEC unary_expr { $$ = new mlang::UnaryOp($1, 0, $2, @$); }
           | TNOT unary_expr { $$ = new mlang::UnaryOp($1, 0, $2, @$); }
           | TMINUS unary_expr { $$ = new mlang::UnaryOp($1, 0, $2, @$); }
           | TPLUS unary_expr { $$ = $2; }
           ;

binop_expr : unary_expr
           | binop_expr TPLUS unary_expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | binop_expr TMINUS unary_expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | binop_expr TMUL unary_expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           | binop_expr TDIV unary_expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
           ;

compare_expr : binop_expr
             | compare_expr TCEQ binop_expr { $$ = new mlang::Comparison($1, $2, $3, @$); }
             | compare_expr TCNE binop_expr { $$ = new mlang::Comparison($1, $2, $3, @$); }
             | compare_expr TCLT binop_expr { $$ = new mlang::Comparison($1, $2, $3, @$); }
             | compare_expr TCLE binop_expr { $$ = new mlang::Comparison($1, $2, $3, @$); }
             | compare_expr TCGT binop_expr { $$ = new mlang::Comparison($1, $2, $3, @$); }
             | compare_expr TCGE binop_expr { $$ = new mlang::Comparison($1, $2, $3, @$); }
             ;

and_expr
	: compare_expr
	| and_expr TAND compare_expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
	;

or_expr
	: and_expr
	| or_expr TOR and_expr { $$ = new mlang::BinaryOp($1, $2, $3, @$); }
	;

ternary_expr
	: or_expr
	| or_expr '?' expr ':' ternary_expr { $$ = new mlang::TernaryOp($1, $3, $5, @$); }
	;

assignment_expr : ternary_expr
                | unary_expr '=' assignment_expr { $$ = new mlang::Assignment($1, $3, @$); }
                ;

expr : assignment_expr
     ;

call_args : %empty  { $$ = new mlang::ExpressionList(); }
          | expr { $$ = new mlang::ExpressionList(); $$->push_back($1); }
          | call_args ',' expr  { $1->push_back($3); }
          ;

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

break : TBREAK { $$ = new mlang::Break(@$); }

free : TFREE expr { $$ = new mlang::FreeMemory($2, @$); }

ident : TIDENTIFIER { $$ = new mlang::Identifier(*$1, @1); delete $1; }
      ;

literals : TINTEGER { $$ = new mlang::Integer($1); }
         | TDOUBLE { $$ = new mlang::Double($1); }
         | TSTR { $$ = new mlang::String(*$1); delete $1; }
         | TBOOL { $$ = new mlang::Boolean($1); }
         | TCHAR { $$ = new mlang::Char($1); }
         ;

conditional : TIF '(' expr ')' block TELSE block { $$ = new mlang::Conditional($3,$5,$7,@$); }
            | TIF '(' expr ')' block { $$ = new mlang::Conditional($3,$5,@$); }
            ;

while : TWHILE '(' expr ')' block { $$ = new mlang::WhileLoop($3,$5,0,@$); }
      | TDO block TWHILE '(' expr ')' { $$ = new mlang::WhileLoop($5,$2,1,@$); }
      ;

for : TFOR '(' ident TIN range ')' block { $$ = new mlang::ForLoop($3, $<range>5, 0, $7, @$); }
    | TFOR '(' ident TIN range TSTEP expr ')' block { $$ = new mlang::ForLoop($3, $<range>5, $7, $9, @$); }
    | TFOR '(' ident TIN expr ')' block { $$ = new mlang::ForEach($3, $5, $7, @$); }
    ;

range : expr TUNTIL expr { $$ = new mlang::Range($1, $2, $3, @$); }
      | expr TTO expr { $$ = new mlang::Range($1, $2, $3, @$); }
      ;

%%
