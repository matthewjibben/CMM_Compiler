%option noyywrap nodefault yylineno
%{
#include "tokens.h"

int chars = 0;
int lines = 0;

/* adjust function, updates the chars value based on the lexeme */
void adjust(){
    chars += yyleng;
}

%}

/*
======= RE CHEAT SHEET =======

.	Any character except newline
\w	word
\d	digit
\s	whitespace
(capital /W, /D, /S stand for Not word, digit, or whitespace)
^	start of string or line
$	end of string or line 

*/
/* regular definitions */

letter	[A-Za-z]
dig	[0-9]
id	{letter}({letter}|{dig})*
number	{dig}+(\.({dig})+)?(E[+-]?({dig})+)?


%%
\s		{ adjust(); /* whitespace, do nothing */ }

\"(.*?)\"       { adjust(); return STR; }


if              { adjust(); return IF; }
else            { adjust(); return ELSE; }
while           { adjust(); return WHILE; }
write           { adjust(); return WRITE; }
writeln         { adjust(); return WRITELN; }


int                     { adjust(); return INT; }
char		            { adjust(); return CHAR; }
string                  { adjust(); return STRING; }


"{"                     { adjust(); return LBRACK; }
"}"                     { adjust(); return RBRACK; }
"("                     { adjust(); return LPAREN; }
")"		                { adjust(); return RPAREN; }


"+"                     { adjust(); return ADD; }
"-"                     { adjust(); return SUB; }
"/"                     { adjust(); return DIV; }
"*"                     { adjust(); return MULT; }
"%"		                { adjust(); return MOD; }

";"		                        { adjust(); return SEMICOLON; }

"=="		                    { adjust(); return EQ; }
"<="		                    { adjust(); return LE; }
">="		                    { adjust(); return GE; }
"<"                             { adjust(); return LT; }
">"                             { adjust(); return GT; }
"!="		                    { adjust(); return NE; }

{id}		                    { adjust(); return ID;
			                    /* lookup if the item exists in the symbol table, if not then create it */ }

{number}	                    { adjust();return INT; }
"="		                        { adjust();return ASSIGN; }


\n		{ adjust(); }
.		{ adjust(); }



%%

int main(int argc, char **argv)
{
    int tok=yylex();
    while (tok!=0) {
        switch(tok){
            case ID:
                printf("<ID, %s>\n", yytext);
                break;
            case INT:
                printf("<INT, %s>\n", yytext);
                break;
            case STR:
                printf("<STR, %s>\n", yytext);
                break;
            default:
                printf("<%s>\n", getTokName(tok));
        }
        //printf("<chars: %i>\n", yyleng);
        tok=yylex();
    }


    printf("Lines: %8d chars: %8d\n", lines, chars);
    printf("yylineno: %i\n", yylineno);
    printf("\n");
}
