%{
#include <stdio.h>
#define YYDEBUG 1
#define YYSTYPE double

int yylex(void);
void yyerror(const char *);
%}
/* token declarations */
%error-verbose

%token ID SEMICOLON COMMA
// %token EQ LE GE LT GT NE
// %token NOT AND OR
%token LBRACK RBRACK LPAREN RPAREN LSQUARE RSQUARE
%token INT CHAR STRING
%token CHARARRAY NUMBER
%token IF THEN ELSE WHILE
%token WRITE WRITELN READ RETRN BREAK
%token ASSIGN MOD //ADD SUB DIV MULT MOD


%left AND OR
%right NOT
%left EQ LE GE LT GT NE
%left ADD SUB
%left MULT DIV

%%

/* Grammar rules */
prog			: VarDecList FunDecList	{ printf("\nsomething happened\n"); }
			;
VarDecList		: /* epsilon */
			| VarDec VarDecList
			;
VarDec			: Type ID SEMICOLON
			| Type ID LSQUARE INT RSQUARE SEMICOLON
			| Type ID ASSIGN Expr SEMICOLON			{ printf(" type: %.10g\n name %.10g\n value: %.10g\n", $1, $2, $4); }
			;
FunDecList		: /* should an epsilon be here? */
			| FunDec
			| FunDec FunDecList
			;
FunDec			: Type ID LPAREN ParamDecList RPAREN Block
			;
ParamDecList		: /* epsilon */
			| ParamDecListTail
			;
ParamDecListTail	: ParamDec
			| ParamDec COMMA ParamDecListTail
			;
ParamDec		: Type ID
			| Type ID LSQUARE RSQUARE
			;
Block			: LBRACK VarDecList StmtList RBRACK
			;
Type			: INT
			| CHAR
			| STRING
			;
StmtList		: Stmt
			| Stmt StmtList
			;
Stmt			: SEMICOLON
			| Expr SEMICOLON
			| RETRN Expr
			| READ ID SEMICOLON
			| WRITE Expr SEMICOLON
			| WRITELN SEMICOLON
			| BREAK SEMICOLON
			| IF LPAREN Expr RPAREN Stmt ELSE Stmt
			| WHILE LPAREN Expr RPAREN Stmt
			| Block
			;
Expr			: Primary
			| UnaryOp Expr
			| Expr BinOp Expr
			| ID ASSIGN Expr
			| ID LSQUARE Expr RSQUARE ASSIGN Expr
			;
Primary			: ID
			| NUMBER
			| LPAREN Expr RPAREN
			| ID LPAREN ExprList RPAREN
			| ID LSQUARE Expr RSQUARE
			;
ExprList		: /* epsilon */
			| ExprListTail
			;
ExprListTail		: Expr
			| Expr COMMA ExprListTail
			;
UnaryOp			: SUB
			| NOT
			;
BinOp			: ADD | SUB | MULT | DIV | EQ | NE | LT | LE | GT | GE | AND | OR
			;

%%

//int main(int argc, char **argv)
//{
//  printf("=======THING============");
//  return yyparse();
//}

void yyerror(const char *s){
    fprintf(stderr, "ERROR: %s\n", s);
}
