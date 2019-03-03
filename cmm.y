%{
#include <stdio.h>
#include <stdlib.h>
#define YYDEBUG 1
#include "ast.h"

int yylex(void);
void yyerror(const char *);
%}

%union {
    int ival;
    char* sval;
    double value;
    struct Expression* expr;
    struct Statement* stmt;
    struct Declaration* decl;
}


/* token declarations */
%error-verbose
//%locations

//%define api.value.type {struct lval}

%token <sval> ID
%token <sval> SEMICOLON COMMA

// %token EQ LE GE LT GT NE
// %token NOT AND OR

%token LBRACK RBRACK LPAREN RPAREN LSQUARE RSQUARE
%token INT
%token CHAR STRING
%token <sval> CHARARRAY SINGLECHAR
%token <ival> NUMBER
%token IF THEN ELSE WHILE
%token WRITE WRITELN READ RETRN BREAK
%token ASSIGN MOD //ADD SUB DIV MULT MOD

%type <ival> Type
%type <decl> VarDec FunDec

%type <expr> Expr Primary Factor Term SimpleExpr Var

%type <sval> AddOp MulOp RelOp

%left AND OR
%right NOT
%left EQ LE GE LT GT NE
%left ADD SUB
%left MULT DIV

// %type<intval> INT
%%
/* new grammar rules */
progam			: DecList			{ printf("\nprogram rule completed\n"); }
			;
DecList			: DecList Declaration
			| /* epsilon */
			;
Declaration		: VarDec
			| FunDec
			;
VarDec			: Type ID SEMICOLON
				{
				$$ = newDeclaration($2, false, $1, NULL, NULL, NULL, NULL, NULL);
				}
			| Type ID ASSIGN Expr SEMICOLON
				{
					//printf("There is a variable declaration\n\n");
					$$ = newDeclaration($2, false, $1, NULL, $4, NULL, NULL, NULL);
					printExpression($4);
					printf("===========%s\n", $$->name);
					//printf(">>>>>>>>>>>>>>>>>%s<<<<<<<<<<<<<<<<<<<<<<<<<<", $2);
				}
			| Type ID LSQUARE NUMBER RSQUARE SEMICOLON
				{
                        	//$$ = newDeclaration($2, true, $1, NULL, NULL, NULL, NULL, NULL);
                        	}
			;
Type			: INT		{ $$ = INT; }
			| CHAR		{ $$ = CHAR; }
			| STRING	{ $$ = STRING; }
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
			| Var ASSIGN Expr
			;
RetrnStmt		: RETRN Expr SEMICOLON			{ }//printf("we have a return"); }
			| RETRN SEMICOLON			{ }//printf("we have a return"); }
			;
WhileStmt		: WHILE LPAREN Expr RPAREN Stmt
			;
IfStmt			: IF LPAREN Expr RPAREN Stmt
			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
			;
Expr			: Primary		{ $$ = $1; }
			| UnaryOp Expr
			| Expr RelOp Expr
				{
				$$ = newExpression(INT, $1, $3, NULL, NULL, $2);
				printExpression($$);
				}
			| Call
			| SimpleExpr
				{
				$$ = $1; //newExpression(INT, NULL, NULL, "simple", NULL, "very simple");
				}
			;



Var			: ID				{$$ = newExpression(NULL, NULL, NULL, $1, NULL, NULL);}
			| ID LSQUARE NUMBER RSQUARE
				{
				//todo check that expression returns an integer
				// the expression is placed on the left side, nothing on the right
				$$ = newExpression(NULL, NULL, NULL, $1, $3, NULL);
				}
			;


Primary			: ID
				{
				printf("there is an ID\n");
				$$ = newExpression(ID, NULL, NULL, $1, NULL, NULL);
				}
			| NUMBER
				{
				$$ = newExpression(INT, NULL, NULL, NULL, $1, NULL);
				printf("there is a number, %i\n", $1);
				}
			| SINGLECHAR
				{
				printf("there is a single character\n");
				$$ = newExpression(SINGLECHAR, NULL, NULL, NULL, NULL, $1);
				}
			| CHARARRAY
				{
				printf("there is a character array\n");
				$$ = newExpression(CHARARRAY, NULL, NULL, NULL, NULL, $1);
				}
			;

/* =============================================== */
RelOp			: EQ  {$$ = "==";}
			| LE  {$$ = "<=";}
			| GE  {$$ = ">=";}
			| LT  {$$ = "<";}
			| GT  {$$ = ">";}
			| NE  {$$ = "!=";}
			| AND {$$ = "&&";}
			| OR  {$$ = "||";}
			;

SimpleExpr		: SimpleExpr AddOp Term
				{
				//todo really we should cec that the two types are compatible, and use that type
				$$ = newExpression(INT, $1, $3, NULL, NULL, $2);
				}

			| Term
				{$$ = $1;}
			;
AddOp			: ADD {$$ = "+";}
			| SUB {$$ = "-";}
			;
UnaryOp			: NOT
			//| SUB				// todo how do we fix unary minus?
			;

Term			: Term MulOp Factor
				{
                                $$ = newExpression(INT, $1, $3, NULL, NULL, $2);
                                }
			| Factor {$$ = $1;}
			;
MulOp 			: MULT {$$ = "*";}
			| DIV  {$$ = "/";}
			;

/* =============================================== */

Factor			: LPAREN Expr RPAREN	{ $$ = $2; }
			| Var
			| Call
			| NUMBER		{$$ = newExpression(INT, NULL, NULL, NULL, $1, NULL);}
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
