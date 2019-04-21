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
FILE* outputTAC;

Env* env;
bool envStartSwitch = false;

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
//				printStatement($1->head, 0);
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
					//SEMANTIC CHECK #6
					// make sure that the assigned expression type matches the vairable
					// if it is a function call, ensure the return type is the same as the variable
					// the symboltable lookup has already been performed in Var
					//  check that the types match, however integers and floats can be set to each other
					if($1 != $4->type && !((($1 == INT) || ($1 == FLOAT)) && (($4->type == INT) || ($4->type == FLOAT))) ){
						char* varType = getTypeString($1);
						char* exprType = getTypeString($4->type);
						semError("Variable of type %s cannot be assigned to %s", varType, exprType);
						YYABORT;
					}

					//SEMANTIC CHECK
					//variable cannot be assigned to an entire array
					if($4->isArray){
						semError("Variable cannot be assigned to an entire array");
						YYABORT;
					}
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
				//SEMANTIC CHECK
				// Function declarations are only allowed inside of the global environment
				if(env->type != 0){
					semError("Error in %s declaration: functions must be in the global scope", $2);
					YYABORT;
				}
				$$ = newDeclaration($2, false, FUNCTION, $1, NULL, NULL, NULL, $4);
				Env* temp = newEnvironment(env);
				env = temp;
				env->type = FUNCTION;
				env->decl = $$;
				envStartSwitch = true;
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
/* the block can have the StmtList. This allows for new functions to be built in a specific scope. */
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
				getBranchWeight($1);
				cgen($1, 0);
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
				//SEMANTIC CHECK #17
				// all break statements must be contained within the body of a loop
				// traverse backwards through each environment and look for a while loop
				Env* tempenv = env;
				while(tempenv!=NULL){
					if(tempenv->type == 0 || tempenv->type == FUNCTION){
						//global environment reached, issue error
						semError("Break statement not inside a loop");
						YYABORT;
					}
					if(tempenv->type == WHILE){
						//while loop found, break
						break;
					}
					tempenv = tempenv->prev;
				}
				$$ = newStatement(STMT_BREAK, NULL, NULL, NULL, NULL, NULL);
				}
			| Block
				{
				$$ = newStatement(STMT_BLOCK, NULL, NULL, $1, NULL, NULL);
				}
			| RetrnStmt
				{
				//SEMANTIC CHECK #7+8
				//Return statements are always inside a function and match the return type
				//traverse backwards through each environment, checking for a function
				//if the global scope is reached, then we are not inside a function
				Env* tempenv = env;
				while(tempenv!=NULL){
					if(tempenv->type == 0){
						//global environment reached, issue error
						semError("Return statement not inside function body");
						YYABORT;
					}
					if(tempenv->type == FUNCTION){
						//function found, check return type
						if($1->expr == NULL) {
							char* declType = getTypeString(tempenv->decl->returnType);
							semError("Expected return type %s, got none", declType);
							YYABORT;
						}
						else if(tempenv->decl->returnType != $1->expr->type){
							char* declType = getTypeString(tempenv->decl->returnType);
							char* exprType = getTypeString($1->expr->type);
							semError("Expected return type %s, got %s", declType, exprType);
							YYABORT;
						}
						break;
					}
					tempenv = tempenv->prev;
				}

				$$=$1;
				}
			| WhileStmt Block
				{
				$1->codeBody = $2;
				$$=$1;
				}
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
WhileStmt		: WHILE LPAREN Expr RPAREN
				{
				//SEMANTIC CHECK #11
				// Expressions used in if and while statements must be either booleans or ints
				// if they are an integer, it is treated that 0=false and non-zero=true
				if($3->type != INT && $3->type != BOOL){
					char* exprType = getTypeString($3->type);
					semError("Expected either integer or boolean in while statement, got %s", exprType);
					YYABORT;
				}

				$$ = newStatement(STMT_WHILE, NULL, $3, NULL, NULL, NULL);
				Env* temp = newEnvironment(env);
                                env = temp;
                                env->type = WHILE;
                                envStartSwitch = true;
				}
			;
IfStmt			: IF LPAREN Expr RPAREN Stmt
				{
				//SEMANTIC CHECK #11
				// Expressions used in if and while statements must be either booleans or ints
				// if they are an integer, it is treated that 0=false and non-zero=true
				if($3->type != INT && $3->type != BOOL){
					char* exprType = getTypeString($3->type);
					semError("Expected either integer or boolean in if statement, got %s", exprType);
					YYABORT;
				}

				$$ = newStatement(STMT_IF, NULL, $3, $5, NULL, NULL);
				}
			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
				{
				//SEMANTIC CHECK #11
				// Expressions used in if and while statements must be either booleans or ints
				// if they are an integer, it is treated that 0=false and non-zero=true
				if($3->type != INT && $3->type != BOOL){
					char* exprType = getTypeString($3->type);
					semError("Expected either integer or boolean in if statement, got %s", exprType);
					YYABORT;
				}

                                $$ = newStatement(STMT_IF_ELSE, NULL, $3, $5, $7, NULL);
                                }
			;

/* =============================================== */

Expr			: Primary		{ $$ = $1; }
			| Expr RelOp Expr	//todo should type be $2? or bool? Answer: bool
				{
				//SEMANTIC CHECK #12
				// Relation operations must use compatible types:
				// Check 1: less than/greater than, etc. must use int or float types
				if($2 == "<=" || $2 == ">=" || $2 == "<" || $2 == ">"){
					if(($1->type != INT) && ($1->type != FLOAT)){
						char* exprType = getTypeString($1->type);
						semError("Unexpected %s in \"%s\" operation, expected either int or float", exprType, $2);
						YYABORT;
					}
					if(($3->type != INT) && ($3->type != FLOAT)){
						char* exprType = getTypeString($3->type);
						semError("Unexpected %s in \"%s\" operation, expected either int or float", exprType, $2);
						YYABORT;
					}
				}
				// Check 2: equal operations must use the same type, but boolean and int equality is allowed
				if($2 == "==" || $2 == "!="){
					if(($1->type != $3->type)){
						// check that we are not comparing ints and booleans, which is allowed
						if( !((($1->type == BOOL) || ($1->type == INT)) && (($3->type == BOOL) || ($3->type == INT))) ){
							char* exprType1 = getTypeString($1->type);
							char* exprType2 = getTypeString($3->type);
							semError("Operation \"%s\" types should be the same, got %s and %s", $2, exprType1, exprType2);
							YYABORT;
						}
					}
				}
				// Check 3: && and || must use either integers or booleans
				if($2 == "&&" || $2 == "||"){
					if(($1->type != INT) && ($1->type != BOOL)){
						char* exprType = getTypeString($1->type);
						semError("Unexpected %s in \"%s\" operation, expected either bool or int", exprType, $2);
						YYABORT;
					}
					if(($3->type != INT) && ($3->type != BOOL)){
						char* exprType = getTypeString($3->type);
						semError("Unexpected %s in \"%s\" operation, expected either bool or int", exprType, $2);
						YYABORT;
					}

				}

				$$ = newExpression(BOOL, $1, $3, NULL, NULL, $2, NULL);
				}
			| Call { $$ = $1; }
			| SimpleExpr {$$ = $1;} // getBranchWeight($1); cgen($1, 0); }
			| Var ASSIGN Expr
				{
				//SEMANTIC CHECK #6
				// make sure that the assigned expression type matches the vairable
				// if it is a function call, ensure the return type is the same as the variable
				// the symboltable lookup has already been performed in Var
				//  check that the types match, however integers and floats can be set to each other
				if($1->type != $3->type && !((($1->type == INT) || ($1->type == FLOAT)) && (($3->type == INT) || ($3->type == FLOAT))) ){
					char* varType = getTypeString($1->type);
					char* exprType = getTypeString($3->type);
					semError("Variable of type %s cannot be assigned to %s", varType, exprType);
					YYABORT;
				}
				//SEMANTIC CHECK
				//variable cannot be assigned to an entire array
				if($3->isArray){
					semError("Variable cannot be assigned to an entire array");
					YYABORT;
				}
				$$ = newExpression(ASSIGN, $1, $3, NULL, NULL, "=", NULL);
				}	// todo should the type be of the assigned variables?
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

				//SEMANTIC CHECK
				// You are only allowed to access array items, not the entire array at once
				// You can only call an array variable with an index
				if(idSymbol->decl->isArray){
					semError("Variable %s is an array, only the items can be accessed using an index", $1);
					YYABORT;
				}

				$$ = newExpression(idSymbol->decl->type, NULL, NULL, $1, NULL, NULL, NULL);
				$$->isArray = idSymbol->decl->isArray;  //not really necessary because arrays must be called with an index


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

				//SEMANTIC CHECK #10:
				// check that the ID is an array
				if(idSymbol->decl->isArray != true){
					semError("Variable %s is not an array", $1);
					YYABORT;
				}
				// check that expression returns an integer
				if($3->type != INT){
					char* exprType = getTypeString($3->type);
					semError("Expected integer in array index, got %s", exprType);
					YYABORT;
				}
				// check that an array is not passed into the index
				if($3->isArray){
					semError("Expected integer in array index, got array");
					YYABORT;
				}

				// the expression is placed on the left side, nothing on the right
				$$ = newExpression(idSymbol->decl->type, $3, NULL, $1, $3->ival, NULL, NULL);			//todo should $3->ival be here?
				//$$->isArray = idSymbol->decl->isArray;


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
				int type;
				if($1->type == FLOAT || $3->type == FLOAT){
					type = FLOAT;
				} else {
					type = INT;
				}
				if($2 =="+"){
					$$ = newExpression(type, $1, $3, NULL, NULL, $2, NULL);
				} else if($2 =="-") {
					$$ = newExpression(type, $1, $3, NULL, NULL, $2, NULL);
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
				int type;
				if($1->type == FLOAT || $3->type == FLOAT){
					type = FLOAT;
				} else {
					type = INT;
				}
				if($2 == "*"){
                                	$$ = newExpression(type, $1, $3, NULL, NULL, $2, NULL);
                                } else if($2 =="/") {
                                	$$ = newExpression(type, $1, $3, NULL, NULL, $2, NULL);
                                }
                                }
			| Factor {$$ = $1;}
			;
MulOp 			: MULT {$$ = "*";}
			| DIV  {$$ = "/";}
			;


Factor			: LPAREN SimpleExpr RPAREN	{ $$ = $2; }
			| Var
				{
				$$ = $1;
				if($$->type == CHAR || $$->type == STRING || $$->type == BOOL) {
					semError("Math expression values must be of type INT or FLOAT or BOOL");
					YYABORT;
				}
				}
			| Call
				{
				$$ = $1;
				if($$->type == CHAR || $$->type == STRING || $$->type == BOOL) {
					semError("Math expression values must be of type INT or FLOAT");
					YYABORT;
				}
				}
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
				Symbol* funcSymbol = lookupFunction(env, $1);
				if(funcSymbol==NULL){
				    semError("Function \"%s\" has not been declared", $1);
				    YYABORT;
				}

				// set the expression type to the return type
				$$->type = funcSymbol->decl->returnType;


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
					for(int i=0; i<$3->size; ++i){
						//check that the types are the same
						if(tempParam->type != tempArg->expr->type){
							char* paramType = getTypeString(tempParam->type);
							char* argType = getTypeString(tempArg->expr->type);
							semError("Parameter type mismatch: %s is not %s", argType, paramType);
							YYABORT;
						}
						// check that isArray matches
						if(tempParam->isArray != tempArg->expr->isArray){
							char* expected = "did not expect array";
							if(tempParam->isArray){expected = "expected array";}
							semError("Parameter array mismatch: %s in parameter %i", expected, i);
							YYABORT;
						}
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