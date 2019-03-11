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
    struct ParamList* params;
    struct ArgList* args;
}


/* token declarations */
%error-verbose
//%define parse.error verbose
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

%type <expr> Expr Primary Factor Term SimpleExpr Var Call

%type <sval> AddOp MulOp RelOp UnaryOp

%type <stmt> Stmt StmtList IfStmt
%type <stmt> Block BlockListTail

%type <params> Params

%type <args> ArgList Args

%left AND OR
%right NOT
%left EQ LE GE LT GT NE
%left ADD SUB
%left MULT DIV
//%precedence UNARY

//%token FUNCTIONCALL

%%
/* new grammar rules */

progam			: StmtList			{ printf("\nprogram rule completed\n"); }
			;
//DecList			: DecList Declaration
//			| /* epsilon */
//			;
Declaration		: VarDec
			| FunDec
			;

/* =============================================== */

VarDec			: Type ID SEMICOLON
				{
				$$ = newDeclaration($2, false, $1, NULL, NULL, NULL, NULL, NULL, NULL);
				}
			| Type ID ASSIGN Expr SEMICOLON
				{
					printf("There is a variable declaration\n\n");
					$$ = newDeclaration($2, false, $1, NULL, NULL, $4, NULL, NULL, NULL);
					printExpression($4, 0);
				}
			| Type ID LSQUARE NUMBER RSQUARE SEMICOLON
				{
                        	$$ = newDeclaration($2, true, $1, NULL, $4, NULL, NULL, NULL, NULL);
                        	}
                        ;
Type			: INT		{ $$ = INT; }
			| CHAR		{ $$ = CHAR; }
			| STRING	{ $$ = STRING; }
			;
FunDec			: Type ID LPAREN Params RPAREN Block
				{
				$$ = newDeclaration($2, false, $1, $1, NULL, NULL, $6, $4, NULL);
				}
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
Block			: LBRACK StmtList RBRACK
			;
//BlockListTail		: BlockListTail StmtList
//			//| BlockListTail StmtList
//			| /* epsilon */
//			;
//BlockList		: DecList StmtList
//			;
StmtList		: StmtList Stmt
			| /* epsilon */
			;

/* =============================================== */

Stmt			: SEMICOLON		//no operation?
			| Expr SEMICOLON	//todo symboltable type lookup
				{
				printExpression($1, 0);
				$$ = newStatement(STMT_EXPR, NULL, $1, NULL, NULL, NULL);
				}
			| READ ID SEMICOLON
			| WRITE Expr SEMICOLON
			| WRITELN SEMICOLON
			| BREAK SEMICOLON
			| Block
			| RetrnStmt
			| WhileStmt
			| IfStmt
			| Declaration
			//| Var ASSIGN Expr SEMICOLON
			;
RetrnStmt		: RETRN Expr SEMICOLON			{ }//printf("we have a return"); }
			| RETRN SEMICOLON			{ }//printf("we have a return"); }
			;
WhileStmt		: WHILE LPAREN Expr RPAREN Stmt
			;
IfStmt			: IF LPAREN Expr RPAREN Stmt
				{
				$$ = newStatement(STMT_IF, NULL, $3, $5, NULL, NULL);
				}
			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
			;

/* =============================================== */

Expr			: Primary		{ $$ = $1; /*printExpression($1, 0);*/ }
			| Expr RelOp Expr
				{
				printf(">>>>>>>>>>>>>>>>RELOP OP<<<<<<<<<<<<<<<<<<<");
				$$ = newExpression(INT, $1, $3, NULL, NULL, $2, NULL);
				//printExpression($$, 0);
				}
			| Call { $$ = $1; }
			| SimpleExpr {$$ = $1;}
			| Var ASSIGN Expr
				{
//				printf("made assign expression");
				$$ = newExpression(ASSIGN, $1, $3, NULL, NULL, "=", NULL);
//				printf("made assign expressioncomplete\n");
				}
			;



Var			: ID			{$$ = newExpression(NULL, NULL, NULL, $1, NULL, NULL, NULL);}  //todo symboltable lookup
			| ID LSQUARE Expr RSQUARE
				{
				// todo check that expression returns an integer
				// the expression is placed on the left side, nothing on the right
				$$ = newExpression(NULL, $3, NULL, $1, $3->ival, NULL, NULL);
				}
			;


Primary			: ID
				{
				//printf("there is an ID\n");
				$$ = newExpression(ID, NULL, NULL, $1, NULL, NULL, NULL);
				}
			| NUMBER
				{
				$$ = newExpression(INT, NULL, NULL, NULL, $1, NULL, NULL);
				//printf("there is a NUMBER, %i\n", $1);
				}
			| SINGLECHAR
				{
				//printf("there is a single character\n");
				$$ = newExpression(SINGLECHAR, NULL, NULL, NULL, NULL, $1, NULL);
				}
			| CHARARRAY
				{
				//printf("there is a character array\n");
				$$ = newExpression(CHARARRAY, NULL, NULL, NULL, NULL, $1, NULL);
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
				//todo really we should check that the two types are compatible, and use that type
				$$ = newExpression(INT, $1, $3, NULL, NULL, $2, NULL);
				}

			| Term
				{$$ = $1;}
			;
AddOp			: ADD {$$ = "+";}
			| SUB {$$ = "-";}
			;
UnaryOp			: NOT {$$ = "!";}
			| SUB {$$ = "-";}
			;

Term			: Term MulOp Factor
				{
                                $$ = newExpression(INT, $1, $3, NULL, NULL, $2, NULL);
                                }
			| Factor {$$ = $1;}
			;
MulOp 			: MULT {$$ = "*";}
			| DIV  {$$ = "/";}
			;


Factor			: LPAREN SimpleExpr RPAREN	{ $$ = $2; }
			| Var				{ $$ = $1; }
			| Call				{ $$ = $1; }
			| NUMBER		{ $$ = newExpression(INT, NULL, NULL, NULL, $1, NULL, NULL); }
			| UnaryOp Factor
				{
                        	$2->ival = -$2->ival;
                        	$$ = $2;
                        	}
			;
Call			: ID LPAREN Args RPAREN		//todo symboltable lookup function type
				{
				// malloc enough size for the new name, including open and close parentheses and null
				char* newName = malloc(strlen($1) + 3);
				if(newName==NULL){exit(1);}	// check malloc error
				strcpy(newName, $1);
				strcat(newName, "()");

				$$ = newExpression(NULL, NULL, NULL, newName, NULL, NULL, $3);
				}
			;
Args			: ArgList		{$$=$1;}
			| /* epsilon */		{$$=NULL;}
			;
ArgList			: Expr COMMA ArgList	{ $$ = newArgList($3, $1); }
			| Expr			{ $$ = newArgList(NULL, $1); }
			;
/* =============================================== */


%%