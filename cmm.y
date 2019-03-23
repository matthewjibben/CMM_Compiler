%{
#include <stdio.h>
#include <stdlib.h>
#define YYDEBUG 1
#include "ast.h"

int yylex(void);
void yyerror(const char *);

FILE* output;
%}

%union {
    int ival;
    char* sval;
    double value;
    float fval;
    struct Expression* expr;
    struct Statement* stmt;
    struct Declaration* decl;

    struct ParamList* params;
    struct Param* param;

    struct ArgList* args;
    struct StatementList* stmtList;
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
%token CHAR STRING BOOL
%token <sval> CHARARRAY SINGLECHAR
%token <ival> NUMBER
%token IF THEN ELSE WHILE
%token WRITE WRITELN READ RETRN BREAK
%token ASSIGN MOD //ADD SUB DIV MULT MOD
%token FLOAT
%token <fval> FLOATVAL

%token <ival> TRUE FALSE

%type <ival> Type
%type <decl> VarDec FunDec Declaration

%type <expr> Expr Primary Factor Term SimpleExpr Var Call Boolean

%type <sval> AddOp MulOp RelOp //UnaryOp

%type <stmt> Stmt IfStmt WhileStmt RetrnStmt
%type <stmt> Block

%type <stmtList> StmtList

%type <params> Params ParamList
%type <param> Param


%type <args> ArgList Args

%left AND OR
%right NOT
%left EQ LE GE LT GT NE
%left ADD SUB
%left MULT DIV
//%precedence UNARY

%token FUNCTION		// this is purely for the parse tree

%%
/* new grammar rules */

progam			: StmtList
				{
				printf("\nprogram rule completed\n\n");
				printStatement($1->head, 0);
				freeStatement($1->head);
				freeStmtList($1);
				}
			;

/* =============================================== */

Declaration		: VarDec	{$$=$1;}
			| FunDec	{$$=$1;}
			;

VarDec			: Type ID SEMICOLON
				{
					$$ = newDeclaration($2, false, $1, NULL, NULL, NULL, NULL, NULL);
				}
			| Type ID ASSIGN Expr SEMICOLON
				{
					$$ = newDeclaration($2, false, $1, NULL, NULL, $4, NULL, NULL);
				}
			| Type ID LSQUARE NUMBER RSQUARE SEMICOLON
				{
                        		$$ = newDeclaration($2, true, $1, NULL, $4, NULL, NULL, NULL);
                        	}
                        ;
Type			: INT		{ $$ = INT; }
			| CHAR		{ $$ = CHAR; }
			| STRING	{ $$ = STRING; }
			| FLOAT		{ $$ = FLOAT; }
			| BOOL		{ $$ = BOOL; }
			;
FunDec			: Type ID LPAREN Params RPAREN Block
				{
				$$ = newDeclaration($2, false, FUNCTION, $1, NULL, NULL, $6, $4);
				}
			;
Params			: ParamList			{$$=$1;}
			| /* epsilon */			{$$=NULL;}
			;
ParamList		: ParamList COMMA Param		{ appendParam($1, $3); }
			| Param				{ $$ = newParamList($1); }
			;
Param			: Type ID			{ $$ = newParam($1, $2, NULL, false); }
			| Type ID LSQUARE RSQUARE	{ $$ = newParam($1, $2, NULL, true); }
			;
/* the block can have the DecList and StmtList. This allows for new functions to be built in a specific scope. */
Block			: LBRACK StmtList RBRACK
				{
				$$ = $2->head;
				freeStmtList($2);
				}
			;
StmtList		: StmtList Stmt
				{
				appendStatement($1, $2);
				}
			| Stmt
				{
				$$ = newStatementList($1);
				}
			| /* epsilon */	{$$=newStatementList(NULL);}
			;

/* =============================================== */

Stmt			: SEMICOLON		//no operation?
				{
				$$ = newStatement(STMT_NULL, NULL,NULL, NULL, NULL, NULL);
				}
			| Expr SEMICOLON	//todo symboltable type lookup
				{
				$$ = newStatement(STMT_EXPR, NULL, $1, NULL, NULL, NULL);
				}
			| READ Var SEMICOLON
				{
				// this can be treated like an assign
				// whatever is given in the console, assign to the ID
				$$ = newStatement(STMT_READ, NULL, $2, NULL, NULL, NULL);
				}
			| WRITE Expr SEMICOLON
				{
				//todo check that this is a string (only print strings)
				$$ = newStatement(STMT_WRITE, NULL, $2, NULL, NULL, NULL);
				}
			| WRITELN SEMICOLON
				{
				$$ = newStatement(STMT_WRITELN, NULL, NULL, NULL, NULL, NULL);
				}
			| BREAK SEMICOLON
				{
				$$ = newStatement(STMT_BREAK, NULL, NULL, NULL, NULL, NULL);
				}
			| Block
				{
				$$ = newStatement(STMT_BLOCK, NULL, NULL, $1, NULL, NULL);
				}
			| RetrnStmt 	{$$=$1;}
			| WhileStmt	{$$=$1;}
			| IfStmt	{$$=$1;}
			| Declaration
				{
				$$=newStatement(STMT_DECL, $1, NULL, NULL, NULL, NULL);
				}
			;
RetrnStmt		: RETRN Expr SEMICOLON	{ $$ = newStatement(STMT_RETRN, NULL, $2, NULL, NULL, NULL); }
			| RETRN SEMICOLON	{ $$ = newStatement(STMT_RETRN, NULL, NULL, NULL, NULL, NULL); }	//could be removed?
			;
WhileStmt		: WHILE LPAREN Expr RPAREN Stmt
				{
				$$ = newStatement(STMT_WHILE, NULL, $3, $5, NULL, NULL);
				}
			;
IfStmt			: IF LPAREN Expr RPAREN Stmt
				{
				$$ = newStatement(STMT_IF, NULL, $3, $5, NULL, NULL);
				}
			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
				{
                                $$ = newStatement(STMT_IF_ELSE, NULL, $3, $5, $7, NULL);
                                }
			;

/* =============================================== */

Expr			: Primary		{ $$ = $1; }
			| Expr RelOp Expr	//todo should type be $2?
				{
				$$ = newExpression($2, $1, $3, NULL, NULL, $2, NULL);
				}
			| Call { $$ = $1; }
			| SimpleExpr {$$ = $1;}
			| Var ASSIGN Expr
				{
				$$ = newExpression(ASSIGN, $1, $3, NULL, NULL, "=", NULL);
				}
			;



Var			: ID			{$$ = newExpression(ID, NULL, NULL, $1, NULL, NULL, NULL);}  //todo symboltable lookup
			| ID LSQUARE Expr RSQUARE
				{
				// todo check that expression returns an integer
				// the expression is placed on the left side, nothing on the right
				$$ = newExpression(ID, $3, NULL, $1, $3->ival, NULL, NULL);
				}
			;


Primary			: Var	{$$ = $1;}
			| NUMBER
				{
				$$ = newExpression(INT, NULL, NULL, NULL, $1, NULL, NULL);
				}
			| SINGLECHAR
				{
				$$ = newExpression(SINGLECHAR, NULL, NULL, NULL, NULL, $1, NULL);
				}
			| CHARARRAY
				{
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

SimpleExpr		: SimpleExpr AddOp Term			//todo symboltable lookup if elements are integers or floats
				{
				//todo really we should check that the two types are compatible, and use that type
				if($2 =="+"){
					$$ = newExpression(ADD, $1, $3, NULL, NULL, $2, NULL);
				} else if($2 =="-") {
					$$ = newExpression(SUB, $1, $3, NULL, NULL, $2, NULL);
				}
				}

			| Term
				{$$ = $1;}
			;
AddOp			: ADD {$$ = "+";}
			| SUB {$$ = "-";}
			;
//UnaryOp			: NOT {$$ = "!";}
                         //			| SUB {$$ = "-";}
                         //			;

Term			: Term MulOp Factor
				{
				if($2 == "*"){
                                	$$ = newExpression(MULT, $1, $3, NULL, NULL, $2, NULL);
                                } else if($2 =="/") {
                                	$$ = newExpression(DIV, $1, $3, NULL, NULL, $2, NULL);
                                }
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
			| FLOATVAL		{ $$ = newExpressionFloat(FLOAT, NULL, NULL, NULL, $1, NULL, NULL); }
			| SUB Factor	//todo update this for NOT
				{
				if($2->type == INT || $2->type == FLOAT){
                        		$2->isUnaryNegate = !$2->isUnaryNegate;
                        		$$ = $2;
				} else {
					semError("Negated value must be of type INT or FLOAT");
					YYABORT;
				}
                        	}
                        | NOT Factor
                        	{
				if($2->type == BOOL || $2->type == INT){
                        		$2->isUnaryNot = !$2->isUnaryNot;
                        		$$ = $2;
				} else {
					semError("Unary NOTed value must be of type BOOL or INT");
					YYABORT;
				}
                        	}
                        | Boolean
			;
Boolean			: TRUE			{ $$ = newExpression(BOOL, NULL, NULL, NULL, 1, NULL, NULL); }
			| FALSE			{ $$ = newExpression(BOOL, NULL, NULL, NULL, 0, NULL, NULL); }
			;
Call			: ID LPAREN Args RPAREN		//todo symboltable lookup function type
				{
				$$ = newExpression(NULL, NULL, NULL, $1, NULL, NULL, $3);
				$$->isFunctionCall = true;
				}
			;
Args			: ArgList		{$$=$1;}
			| /* epsilon */		{$$=NULL;}
			;
ArgList			: ArgList COMMA Expr	{ appendArgument($1, newArgument(NULL, $3)); }
			| Expr
				{
				$$ = newArgList(newArgument(NULL, $1));
				}
			;
/* =============================================== */


%%