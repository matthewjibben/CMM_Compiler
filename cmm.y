%{
#include <stdio.h>
//#include "lval.h"
#define YYDEBUG 1
//#define YYSTYPE double

int yylex(void);
void yyerror(const char *);
%}

%union {
    int ival;
    char* sval;
    double value;
}

/* token declarations */
%error-verbose
%locations

//%define api.value.type {struct lval}

%token <sval> ID
%token <sval> SEMICOLON COMMA

// %token EQ LE GE LT GT NE
// %token NOT AND OR

%token LBRACK RBRACK LPAREN RPAREN LSQUARE RSQUARE
%token INT
%token CHAR STRING
%token CHARARRAY SINGLECHAR
%token NUMBER
%token IF THEN ELSE WHILE
%token WRITE WRITELN READ RETRN BREAK
%token ASSIGN MOD //ADD SUB DIV MULT MOD

%type <ival> Type
%type <ival> VarDec

%left AND OR
%right NOT
%left EQ LE GE LT GT NE
%left ADD SUB
%left MULT DIV

// %type<intval> INT
%%
/* new grammar rules */
progam			: DecList			{ printf("program complete\n"); }
			;
DecList			: DecList Declaration
			| /* epsilon */
			;
Declaration		: VarDec
			| FunDec
			;
VarDec			: Type ID SEMICOLON
			| Type ID ASSIGN Expr SEMICOLON
			| Type ID LSQUARE NUMBER RSQUARE SEMICOLON
			;
Type			: INT
			| CHAR
			| STRING
			;
FunDec			: Type ID LPAREN Params RPAREN Block
			;
Params			: ParamList
			| /* epsilon */
			;
ParamList		: ParamList COMMA Param
			| Param
			;
Param			: Type ID
			| Type ID LSQUARE RSQUARE
			;
/* the block can have the DecList and StmtList. This allows for new functions to be built in a specific scope. */
Block			: LBRACK BlockListTail RBRACK
			;
BlockListTail		: BlockListTail BlockList
			| /* epsilon */
			;
BlockList		: DecList StmtList
			;
StmtList		: StmtList Stmt
			| /* epsilon */
			;
Stmt			: SEMICOLON
			| Expr SEMICOLON
			| READ ID SEMICOLON
			| WRITE Expr SEMICOLON
			| WRITELN SEMICOLON
			| BREAK SEMICOLON
			| Block
			| RetrnStmt
			| WhileStmt
			| IfStmt
			;
RetrnStmt		: RETRN Expr SEMICOLON
			| RETRN SEMICOLON
			;
WhileStmt		: WHILE LPAREN Expr RPAREN Stmt
			;
IfStmt			: IF LPAREN Expr RPAREN Stmt
			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
			;
Expr			: Primary
			| UnaryOp Expr
			| Expr RelOp Expr
			| Var ASSIGN Expr
			| Call
			| SimpleExpr
			;
UnaryOp			: SUB
			| NOT
			;
Var			: ID
			| ID LSQUARE Expr RSQUARE
			;
Primary			: ID					{ printf("there is an ID\n"); }
			| NUMBER				{ printf("there is a number\n"); }
			| SINGLECHAR				{ printf("there is a single character\n"); }
			| CHARARRAY				{ printf("there is a character array\n"); }
			//| LPAREN Expr RPAREN
			//| ID LPAREN ExprList RPAREN
			//| ID LSQUARE Expr RSQUARE
			;

SimpleExpr		: AddExpr RelOp AddExpr
			| AddExpr
			;
RelOp			: EQ | LE | GE | LT | GT | NE
			;
AddExpr			: AddExpr AddOp Term
			| Term
			;
AddOp			: ADD | SUB
			;
Term			: Term MulOp Factor
			| Factor
			;
MulOp 			: MULT
			| DIV
			;
Factor			: LPAREN Expr RPAREN
			| Var
			| Call
			| NUMBER
			;
Call			: ID LPAREN Args RPAREN
			;
Args			: ArgList
			| /* epsilon */
			;
ArgList			: ArgList COMMA Expr
			| Expr
			;




/* Grammar rules */
//prog			: VarDecList FunDecList	{ printf("\n=parse tree completed, return AST=\n"); }
//			;
//VarDecList		: VarDec VarDecList	{printf("THAR BE THE ANSWER %i\n", $1);}
//			| /* epsilon */
//			;
//VarDec			: Type ID SEMICOLON
//			| Type ID LSQUARE INT RSQUARE SEMICOLON
//			{ printf("there is an assignment\n"); $$ = 123; }// printf(" type: %.10g\n name \n value: \n", $1);
//			 					  //printf("ID name: %s\n", "fake");}
//			;
////Var_decl		:
//
//FunDecList		: FunDec
//			| FunDec FunDecList
//			;
//FunDec			: Type ID LPAREN ParamDecList RPAREN Block
//			;
//ParamDecList		: ParamDecListTail
//			| /* epsilon */
//			;
//ParamDecListTail	: ParamDec
//			| ParamDec COMMA ParamDecListTail
//			;
//ParamDec		: Type ID
//			| Type ID LSQUARE RSQUARE
//			;
//Block			: LBRACK VarDecList StmtList RBRACK
//			;
//Type			: INT						{}
//			| CHAR		{}
//			| STRING	{}
//			;
//StmtList		: Stmt
//			| Stmt StmtList
//			;
//Stmt			: SEMICOLON
//			| Expr SEMICOLON
//			| RETRN Expr
//			| READ ID SEMICOLON
//			| WRITE Expr SEMICOLON
//			| WRITELN SEMICOLON
//			| BREAK SEMICOLON
//			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
//			| WHILE LPAREN Expr RPAREN Stmt
//			| Block
//			;
//Expr			: Primary
//			| UnaryOp Expr
//			| Expr BinOp Expr
//			| ID ASSIGN Expr
//			| ID LSQUARE Expr RSQUARE ASSIGN Expr
//			;
//Primary			: ID					{ printf("there is an ID\n"); }
//			| NUMBER				{ printf("there is a number\n"); }
//			| SINGLECHAR				{ printf("there is a single character\n"); }
//			| CHARARRAY				{ printf("there is a character array\n"); }
//			| LPAREN Expr RPAREN
//			| ID LPAREN ExprList RPAREN
//			| ID LSQUARE Expr RSQUARE
//			;
//ExprList		: ExprListTail
//			| /* epsilon */
//			;
//ExprListTail		: Expr
//			| Expr COMMA ExprListTail
//			;
//UnaryOp			: SUB
//			| NOT
//			;
//BinOp			: ADD | SUB | MULT | DIV | EQ | NE | LT | LE | GT | GE | AND | OR
//			;

%%

//int main(int argc, char **argv)
//{
//  printf("=======THING============");
//  return yyparse();
//}
