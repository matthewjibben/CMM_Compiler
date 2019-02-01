%{
int chars = 0;
int words = 0;
int lines = 0;
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

\s		{ /* whitespace, do nothing */ }
if|else|while|print|println	{ printf("<%s> ", yytext); chars+=strlen(yytext); /* statements */ }
int|char|string		{ printf("<type, %s> ", yytext); chars+=strlen(yytext); /* datatypes */ }
\{|\}|\(|\)		{ printf("<%s> ",yytext); chars++; /* containers */ }
\+|\-|\/|\*|%		{ printf("<%s> ", yytext); chars++; /* arithmetic */ }
";"		{ printf("<;> "); chars++; }
={2}		{ printf("<EQ> "); chars+=2; }
"<="		{ printf("<LE> "); chars++; }
">="		{ printf("<GE> "); chars++; } 
"<"             { printf("<LT> "); chars++; } 
">"             { printf("<GT> "); chars++; } 
"!="		{ printf("<NE> "); chars+=2; }

{id}		{ printf("<id, \"%s\"> ", yytext); chars+=strlen(yytext); 
			/* lookup if the item exists in the symbol table, if not then create it */ }
{number}	{ printf("<num, %s> ", yytext); chars+=strlen(yytext); }
=		{ printf("<assign> "); chars++; }

\n		{ chars++; lines++; }
.		{ chars++; }



%%

int main(int argc, char **argv)
{
  yylex();
  //printf("%8d%8d%8d\n",lines, words, chars);
  printf("\n");
}
