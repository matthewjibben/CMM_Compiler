//
// Created by Matthew Jibben on 2/2/2019.
//

#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

/* File no longer needed
enum yytokentype
{
    ID      =   257,
    //reserved words
    INT     =   258,
    CHAR    =   259,

    IF      =   260,
    THEN    =   261,
    ELSE    =   262,
    WHILE   =   263,

    WRITE   =   264,
    WRITELN =   265,
    READ    =   266,
    RETRN   =   267,
    BREAK   =   268,

    // Comparisons
    EQ      =   269,
    LE      =   270,
    GE      =   271,
    LT      =   272,
    GT      =   273,
    NE      =   274,

    // Others
    ASSIGN  =   275,
    LBRACK  =   276,
    RBRACK  =   277,
    LPAREN  =   278,
    RPAREN  =   279,

    ADD     =   280,
    SUB     =   281,
    DIV     =   282,
    MULT    =   283,
    SEMICOLON   =   284,
    MOD     =   285,

    STRING  =   286,
    CHARARRAY     =   287,
    LSQUARE =   288,
    RSQUARE =   289,
    COMMA   =   290,
    NUMBER  =   291,
    NOT     =   292,
    AND     =   293,
    OR      =   294
};

char* tokennames[] = {
    "ID",
    "INT",
    "CHAR",
    "IF",
    "THEN",
    "ELSE",
    "WHILE",
    "WRITE",
    "WRITELN",
    "READ",
    "RETRN",
    "BREAK",
    "EQ",
    "LE",
    "GE",
    "LT",
    "GT",
    "NE",
    "ASSIGN",
    "LBRACK",
    "RBRACK",
    "LPAREN",
    "RPAREN",
    "ADD",
    "SUB",
    "DIV",
    "MULT",
    "SEMICOLON",
    "MOD",
    "STRING",
    "CHARARRAY",
    "LSQUARE",
    "RSQUARE",
    "COMMA",
    "NUMBER",
    "NOT",
    "AND",
    "OR"
};

char* getTokName(int token){
    return tokennames[token-257];
}

*/

#endif //TOKENS_H
