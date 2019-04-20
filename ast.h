//
// Created by Matthew Jibben on 2/22/2019.
//

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmm.tab.h"


/* ============================================== */


typedef struct ParamList{
    struct Param* head;
    struct Param* tail;
    int size;
} ParamList;

typedef struct Param {
    int type;
    char* name;
    struct Param* next;

    bool isArray;
} Param;



typedef struct ArgList {
    struct Argument* head;
    struct Argument* tail;
    int size;
} ArgList;

typedef struct Argument {
    struct Argument* next;
    struct Expression* expr;
} Argument;

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
    //struct Declaration* next;       // this is the next declaration
} Declaration;


/* ============================================== */


typedef enum stmtType{
    STMT_EXPR,
    STMT_READ,
    STMT_WRITE,
    STMT_WRITELN,
    STMT_BREAK,
    STMT_BLOCK,
    STMT_RETRN,
    STMT_WHILE,
    STMT_IF,
    STMT_IF_ELSE,
    STMT_DECL,
    STMT_NULL
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
    bool isArray;
    struct Expression* left;
    struct Expression* right;

    char* name;
    int ival;
    char* sval;
    float fval;

    bool isFunctionCall;

    bool isUnaryNegate;
    bool isUnaryNot;

    ArgList* args;

    int weight;     // this is used in IR generation for tree translation

} Expression;


/* ============================================== */

//main struct builder functions
Declaration* newDeclaration(char* name, bool isArray, int type, int returnType, int size,
                            Expression* value, Statement* codeBlock, ParamList* params);
Statement* newStatement(stmtType type, Declaration* decl, Expression* expr,
                        Statement* codeBody, Statement* elseBody, Statement* next);
Expression* newExpression(int type, Expression* left, Expression* right, char* name, int ival, char* sval,
        ArgList* args);


Expression* newExpressionFloat(int type, Expression* left, Expression* right, char* name, float fval, char* sval,
                               ArgList* args);
/* ================= */


ArgList* newArgList(Argument* arg);
Argument* newArgument(Argument* next, Expression* expr);
void appendArgument(ArgList* argList, Argument* arg);

/* ================= */

StatementList* newStatementList(Statement* stmt);
void appendStatement(StatementList* stmtList, Statement* stmt);
void printStatementList(StatementList* stmtList, int indent);

/* ================= */
void printExpression(Expression* expr, int indent);
void printArgList(ArgList* args, int indent);
void printStatement(Statement* stmt, int indent);
void printDeclaration(Declaration* decl, int indent);
void printParams(ParamList* params);
void printIndent(int indent);

void printTypeString(int typeToken);
char* getTypeString(int typeToken);
/* ================= */

Param* newParam(int type, char* name, Param* next, bool isArray);
ParamList* newParamList(Param* param);
void appendParam(ParamList* paramList, Param* param);

/* ================= */
// struct freeing functions
void freeStmtList(StatementList* stmtList);

void freeExpression(Expression* expr);
void freeDeclaration(Declaration* decl);
void freeStatement(Statement* stmt);

void freeArgList(ArgList* argList);
void freeArgument(Argument* arg);

void freeParamList(ParamList* params);
void freeParameter(Param* param);


#endif //AST_H
