//
// Created by Matthew Jibben on 2/2/2019.
//

#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

enum yytokentype
{
    ID      =   257,
    /* reserved words */
    INT     =   258,
    CHAR    =   259,

    IF      =   260,
    THEN    =   261,
    ELSE    =   262,
    WHILE   =   263,

    WRITE   =   264,
    WRITELN   =   265,
    READ    =   266,
    RETRN   =   267,
    BREAK   =   268,

    /* Comparisons */
    EQ      =   269,
    LE      =   270,
    GE      =   271,
    LT      =   272,
    GT      =   273,
    NE      =   274,

    /* Others */
    ASSIGN   =   275,
    LBRACK  =   276,
    RBRACK  =   277,
    LPAREN  =   278,
    RPAREN  =   279,

    /* Other */
    ADD     =   280,
    SUB     =   281,
    DIV     =   282,
    MULT    =   283,
    SEMICOLON   =   284,
    MOD     =   285,

    STRING  =   286,
    STR     =   287
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
    "STR"
};

char* getTokName(int token){
    return tokennames[token-257];
}



#endif //TOKENS_H
