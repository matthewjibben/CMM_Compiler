%option noyywrap nodefault yylineno
%{
#include "cmm.tab.h"
//#include "tokens.h"
#include "symboltable.h"



int chars = 0;

// declare the global environment
struct Env* env;

/* adjust function, updates the chars value based on the lexeme */
void adjust(){
    chars += yyleng;
}

%}

/*
======= RE CHEAT SHEET ========

.	Any character except newline
\w	word
\d	digit
\s	whitespace
(capital /W, /D, /S stand for Not word, digit, or whitespace)
^	start of string or line
$	end of string or line 
===============================
*/

/* regular definitions */

letter	[A-Za-z]
dig	[0-9]
id	{letter}({letter}|{dig})*
number	{dig}+(\.({dig})+)?(E[+-]?({dig})+)?


%%
[ \t]+  		{ adjust(); /* whitespace, do nothing */}

\"(.*?)\"       { adjust(); return CHARARRAY; }


if              { adjust(); return IF; }
else            { adjust(); return ELSE; }
while           { adjust(); return WHILE; }
write           { adjust(); return WRITE; }
writeln         { adjust(); return WRITELN; }


int                     { adjust(); return INT; }
char		            { adjust(); return CHAR; }
string                  { adjust(); return STRING; }


"{"                     { adjust();

                          // create new environment and enter it
                          struct Env* temp = malloc(sizeof(struct Env));
                          initEnv(temp, env);
                          env = temp;
                          printf("entering new scope...\n");

                          return LBRACK; }
"}"                     { adjust();

                          //print the environment and then delete it
                          printf("exiting environment:\n");
                          printEnv(env);

                          //deleting
                          struct Env* temp = malloc(sizeof(struct Env));
                          temp = env;
                          env = env->prev;
                          freeEnv(temp);

                          return RBRACK; }

"("                     { adjust(); return LPAREN; }
")"		                { adjust(); return RPAREN; }

"["                     { adjust(); return LSQUARE; }
"]"                     { adjust(); return RSQUARE; }

"+"                     { adjust(); return ADD; }
"-"                     { adjust(); return SUB; }
"/"                     { adjust(); return DIV; }
"*"                     { adjust(); return MULT; }
"%"		                { adjust(); return MOD; }

";"		                        { adjust(); return SEMICOLON; }
","                             { adjust(); return COMMA; }

"=="		                    { adjust(); return EQ; }
"<="		                    { adjust(); return LE; }
">="		                    { adjust(); return GE; }
"<"                             { adjust(); return LT; }
">"                             { adjust(); return GT; }
"!="		                    { adjust(); return NE; }

"!"                             { adjust(); return NOT; }
"&&"                             { adjust(); return AND; }
"||"                             { adjust(); return OR; }


{id}		                    { adjust(); return ID; }

{number}	                    { adjust();return NUMBER; }
"="		                        { adjust();return ASSIGN; }


\n		{ adjust(); }
.		{ adjust(); printf("unknown character on line %i\n", yylineno); }



%%

//int main(int argc, char **argv)
//{
//    printf("> ");
//    printf("================================================================================\n");
//    printf("================================================================================\n");
//    printf("================================================================================\n");
//    //return yyparse();
//}

//int main(int argc, char **argv)
//{
//    // initialize the global environment using malloc
//    env = malloc(sizeof(struct Env));
//    initEnv(env, NULL);
//
//    int tok=yylex();
//    while (tok!=0) {
//        switch(tok){
//            case ID:
//                printf("<ID, %s>\n", yytext);
//                /* lookup if the item exists in the symbol table, if not then create it */
//                /* for the lexer, I will only add the item to the symbol table */
//                printf("inserting...\n");
//                char* temp = strdup(yytext);
//                insertEntry(env, temp, NULL, NULL);
//                break;
//            case NUMBER:
//                printf("<NUMBER, %s>\n", yytext);
//                break;
//            case CHARARRAY:
//                printf("<CHARARRAY, %s>\n", yytext);
//                break;
//            default:
//                printf("<%s>\n", getTokName(tok));
//        }
//        //printf("<chars: %i>\n", yyleng);
//        tok=yylex();
//    }
//
//    printf("finished, show global environment: \n");
//    printEnv(env);
//
//
//    printf("chars: %8d\n", chars);
//    printf("yylineno: %i\n", yylineno);
//    printf("\n");
//}
