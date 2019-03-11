//
// Created by Matthew Jibben on 2/22/2019.
//

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



/* ============================================== */


typedef struct ParamList{
    struct ParamList* next;
    int type;
    char* name;
} ParamList;

typedef struct ArgList {
    struct ArgList* next;
    struct Expression* expr;
} ArgList;


/* ============================================== */


typedef struct Declaration{
    char* name;                     // variable name
    bool isArray;
    int size;

    int type;

    int returnType;                 // only used for function declarations

    struct Expression* value;        // this is the expression that is assigned to the variable
    struct Statement* codeBlock;    // used for function assignments
    struct ParamList* params;       // this is the list of parameters in a function declaration
    //todo should the size of the ParamList be included?
    struct Declaration* next;       // this is the next declaration
} Declaration;


/* ============================================== */


typedef enum stmtType{
    STMT_IF,
    STMT_IF_ELSE,
    STMT_BLOCK,
    STMT_WHILE,
    STMT_RETRN,
    STMT_EXPR,
    STMT_DECL
} stmtType;

typedef struct Statement{
    stmtType type;
    struct Declaration* decl;           // declaration statement
    struct Expression* expr;            // expression statement

    struct Statement* codeBody;         // first code body
    struct Statement* elseBody;         // second code body for if/else statements

    struct Statement* next;
} Statement;


typedef struct StatementList{
    struct Statement* head;
    struct Statement* tail;
} StatementList;


/* ============================================== */


typedef struct Expression{
    int type;
    struct Expression* left;
    struct Expression* right;

    char* name;
    int ival;
    char* sval;

    ArgList* args;
} Expression;


/* ============================================== */


Declaration* newDeclaration(char* name, bool isArray, int type, int returnType, int size,
                            Expression* value, Statement* codeBlock, ParamList* params, Declaration* next);
Statement* newStatement(stmtType type, Declaration* decl, Expression* expr,
                        Statement* codeBody, Statement* elseBody, Statement* next);
Expression* newExpression(int type, Expression* left, Expression* right, char* name, int ival, char* sval,
        ArgList* args);
ArgList* newArgList(ArgList* next, Expression* expr);

/* ================= */

void appendStatement(StatementList* stmtList, Statement* stmt);

/* ================= */
void printExpression(Expression* expr, int indent);
void printArgList(ArgList* args, int indent);


#endif //AST_H
