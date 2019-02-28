//
// Created by Matthew Jibben on 2/22/2019.
//

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//typedef struct {
//    int size;
//    char* value;
//
//    // pointers to the linked list of child nodes
//    // having a linked list of children allows for any number of children
//    // in binary expressions, only left and right will be used
//    struct AST* left;
//    struct AST* right;
//
//    // pointers to the nodes on the same level (siblings)
//    struct AST* next;
//    //struct AST* prev;
//} AST;
//
//typedef struct {
//    AST* head;
//} ASTHead;


/* ============================================== */

typedef struct paramList{
    struct paramList* next;
    int type;
    char* name;
} paramList;

typedef struct Declaration{
    char* name;                     // variable name
    bool isArray;
//    int arraySize;                  //todo should arraysize be included for semantic analysis?

    int type;

    int returnType;                 // only used for function declarations

    struct Expression* value;        // this is the expression that is assigned to the variable
    struct Statement* codeBlock;    // used for function assignments
    struct paramList* params;       // this is the list of parameters in a function declaration
    //todo should the size of the paramlist be included?
    struct Declaration* next;       // this is the next declaration
} Declaration;

/* ============================================== */

typedef enum stmtType{
    STMT_IF,
    STMT_IF_ELSE,
    STMT_BLOCK,
    STMT_WHILE,
    STMT_RETRN
} stmtType;

typedef struct Statement{
    stmtType type;
    struct Declaration* decl;           // declaration statement
    struct Expression* expr;            // expression for if statements

    struct Statement* codeBody;         // first code body
    struct Statement* elseBody;         // second code body for if/else statements

    struct Statement* next;
} Statement;

/* ============================================== */

typedef struct Expression{
    int type;
    struct Expression* left;
    struct Expression* right;

    char* name;
    int ival;
    char* sval;
} Expression;

Declaration* newDeclaration(char* name, bool isArray, int type, int returnType,
                            Expression* value, Statement* codeBlock, paramList* params, Declaration* next);
Statement* newStatement(stmtType type, Declaration* decl, Expression* expr,
                        Statement* codeBody, Statement* elseBody, Statement* next);
Expression* newExpression(int type, Expression* left, Expression* right, char* name, int ival, char* sval);
void printExpression(Expression* expr);
//void initAST(AST* ast, char* value);
//void addChild(AST* parent, AST* child);
//void printAST(AST* head);



#endif //AST_H
