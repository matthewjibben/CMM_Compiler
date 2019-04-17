%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define YYDEBUG 1
#include "ast.h"
#include "symboltable.h"

int yylex(void);
void yyerror(const char*);
void semError(const char*, ...);
FILE* output;

Env* env;
bool funcDeclarationSwitch = false;

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
				//printStatement($1->head, 0);
				//print global environment
				printEnv(env);

				freeStatement($1->head);
				freeStmtList($1);
				}
			;

/* =============================================== */

Declaration		: VarDec	{$$=$1;}
			| FunDec Block
				{
				$1->codeBlock = $2;
				$$=$1;
				}
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
					//SEMANTIC CHECK #4
					//The int identifier cannot be 0 or negative
					if($4 <= 0){
						semError("Array size must be greater than 0");
						YYABORT;
					}
                        		$$ = newDeclaration($2, true, $1, NULL, $4, NULL, NULL, NULL);
                        	}
                        ;
Type			: INT		{ $$ = INT; }
			| CHAR		{ $$ = CHAR; }
			| STRING	{ $$ = STRING; }
			| FLOAT		{ $$ = FLOAT; }
			| BOOL		{ $$ = BOOL; }
			;
FunDec			: Type ID LPAREN Params RPAREN
				{
				$$ = newDeclaration($2, false, FUNCTION, $1, NULL, NULL, NULL, $4);
				Env* temp = newEnvironment(env);
				env = temp;
				funcDeclarationSwitch = true;
				// add the parameters to the new environment
				if($4 != NULL){
					Param* temp = $4->head;
					while(temp!=NULL){
						// create a declaration of the parameter and add to the current scope
						Declaration* paramDec = newDeclaration(temp->name, temp->isArray, temp->type, NULL, NULL, NULL, NULL, NULL);
						insertEntry(env, paramDec);
						temp = temp->next;
					}
				}
				}
			;
Params			: ParamList			{$$=$1;}
			| /* epsilon */			{$$=newParamList(NULL);}
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
				//SEMANTIC CHECK #1:
				// check that the declared variable has not been previously declared in the same scope
				if(lookupCurrentEnv(env, $1->name)!=NULL){
					semError("Value %s cannot be declared with the same name twice", $1->name);
					YYABORT;
				}


				// create the statement and add the declaration to the current environment
				insertEntry(env, $1);
//				printDeclaration($1, 0);
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



Var			: ID
				{
				//SEMANTIC CHECK #2:
				//No ID can be used before it is declared
				//do a full symbol table lookup and check that the ID is declared
				Symbol* idSymbol = lookup(env, $1);
				if(idSymbol==NULL){
				    semError("Variable %s has not been declared", $1);
				    YYABORT;
				}


				$$ = newExpression(idSymbol->decl->type, NULL, NULL, $1, NULL, NULL, NULL);
				$$->isArray = idSymbol->decl->isArray;


				}  //todo symboltable lookup
			| ID LSQUARE Expr RSQUARE
				{
				//SEMANTIC CHECK #2:
				//No ID can be used before it is declared
				//do a full symbol table lookup and check that the ID is declared
				Symbol* idSymbol = lookup(env, $1);
				if(idSymbol==NULL){
				    semError("Variable %s has not been declared", $1);
				    YYABORT;
				}

				// todo check that expression returns an integer
				// the expression is placed on the left side, nothing on the right
				$$ = newExpression(idSymbol->decl->type, $3, NULL, $1, $3->ival, NULL, NULL);			//todo should $3->ival be here?
				$$->isArray = idSymbol->decl->isArray;


				}
			;


Primary			: Var	{$$ = $1;}
			| NUMBER
				{
				$$ = newExpression(INT, NULL, NULL, NULL, $1, NULL, NULL);
				}
			| SINGLECHAR
				{
				$$ = newExpression(CHAR, NULL, NULL, NULL, NULL, $1, NULL);
				}
			| CHARARRAY
				{
				$$ = newExpression(STRING, NULL, NULL, NULL, NULL, $1, NULL);
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

				//SEMANTIC CHECK #2:
				//No ID can be used before it is declared
				//do a full symbol table lookup and check that the ID is declared
				Symbol* funcSymbol = lookup(env, $1);
				if(funcSymbol==NULL){
				    semError("Function \"%s\" has not been declared", $1);
				    YYABORT;
				}


				//SEMANTIC CHECK #5:
				// function call arguments must match the function declaration parameters
				// first check that the argument and parameter sizes are the same
				if(funcSymbol->decl->params->size != $3->size){
					semError("Function call (%i) must have the same number of parameters as the function declaration (%i)", $3->size, funcSymbol->decl->params->size);
					YYABORT;
				}
				//loop through the arglist and the paramlist, checking each type
				if($3->size > 0){
					Argument* tempArg = $3->head;
					Param* tempParam = funcSymbol->decl->params->head;
					//at this point we know that both have the same size
					while(tempArg != NULL){
						//check that the types are the same
						if(tempParam->type != tempArg->expr->type){
							char* paramType = getTypeString(tempParam->type);
							char* argType = getTypeString(tempArg->expr->type);
//							semError("no");
							semError("Parameter type mismatch: %s is not %s", argType, paramType);
							YYABORT;
						}
						//do a symbol table lookup to get the variable if it is one

						tempArg = tempArg->next;
						tempParam = tempParam->next;
					}

				}
				}
			;
Args			: ArgList		{$$=$1;}
			| /* epsilon */		{$$=newArgList(NULL);}
			;
ArgList			: ArgList COMMA Expr	{
						appendArgument($1, newArgument(NULL, $3));
						}
			| Expr
				{
				$$ = newArgList(newArgument(NULL, $1));
				}
			;
/* =============================================== */


%%