%{
#include "symboltable.h"
#include "cmm.tab.h"
#include <stdarg.h>


int chars = 0;

// declare the global environment
//struct Env* env;
//env = newEnvironment(NULL);

extern bool envStartSwitch;
extern Env* env;

/* adjust function, updates the chars value based on the lexeme */
void adjust(){
    chars += yyleng;
}

%}

%option noyywrap nodefault yylineno
/* %option bison-locations This causes issues with yylval, do not use*/

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
[ \t\r\f]  		{ adjust(); /* whitespace, do nothing */ }
\/[\/]+.*           { adjust(); /* code comment, do nothing */ }

\"[^\"\n]\"     { adjust();
                    char buff[5];
                    snprintf(buff, 5, "\'%c\'", yytext[1]);     // copy only the character value
                    yylval.sval = strdup(buff);
                    return SINGLECHAR; }
\"([^\"\n]*)\"  { adjust(); yylval.sval = strdup(yytext); return CHARARRAY; }


if              { adjust(); return IF; }
else            { adjust(); return ELSE; }
while           { adjust(); return WHILE; }
read            { adjust(); return READ; }
write           { adjust(); return WRITE; }
writeln         { adjust(); return WRITELN; }
return          { adjust(); return RETRN; }
break           { adjust(); return BREAK; }

float           { adjust(); return FLOAT; }
bool            { adjust(); return BOOL; }

true            { adjust(); return TRUE; }
false           { adjust(); return FALSE; }


int                     { adjust(); return INT; }
char		            { adjust(); return CHAR; }
string                  { adjust(); return STRING; }


"{"                     { adjust();
                          // only enter the new environment if the envStartSwitch is false
                          // this ensures that the function parameters are stored in the right environment
                          if(envStartSwitch){
                                envStartSwitch = false;
                          } else {
                              // create new environment and enter it
                              Env* temp = newEnvironment(env);
                              env = temp;
                              env->type = 1;
                              printf("entering new scope...\n");
                          }

                          return LBRACK; }
"}"                     { adjust();

                          //print the environment and then delete it
                          printf("exiting environment:\n");
                          printEnv(env);

                          //deleting
                          Env* temp = env;
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

;		                        { adjust(); return SEMICOLON; }
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


{id}		                    { adjust();
                                    yylval.sval = strdup(yytext);
                                    return ID;
                                    }

{dig}*\.{dig}*                  { adjust();
                                    yylval.fval = atof(yytext);
                                    return FLOATVAL; }

{number}	                    { adjust();
                                    yylval.ival = atoi(yytext);
                                    return NUMBER; }


"="		                        { adjust();return ASSIGN; }


\n		{ adjust(); chars = 0; }
.		{ adjust(); /* int x = (int)yytext[0]; printf("unknown character on line %i, index: %i, asciival: %i\n", yylineno, chars, x); */ }


%%



void yyerror(const char *s){
    fprintf(stderr, "SYNTAX ERROR on line %i, character: %i: %s\n", yylineno, chars, s);
    printf("current word: %s", yytext);
}

void semError(const char* message, ...){
    //get the list of arguments to include in the message
    va_list args;
    va_start(args, message);

    printf("SEMANTIC ERROR on line %i, character %i: ", yylineno, chars);
    vprintf(message, args);
    printf("\n");
    printf("current word: %s\n\n", yytext);

    va_end(args);
}

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
