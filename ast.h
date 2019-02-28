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
typedef struct {
    struct paramList* next;
    int type;
    char* name;
} paramList;

typedef struct {
    char* name;
    bool isArray;

    int type;

    int returnType;

    struct Expression* expr;
    struct Statement* codeBlock;
    struct Declaration* next;
} Declaration;

/* ============================================== */

typedef enum {
    STMT_IF,
    STMT_IF_ELSE,
    STMT_BLOCK,
    STMT_WHILE,
    STMT_RETRN
} stmtType;

typedef struct {
    stmtType type;
    struct Declaration* decl;
    struct Expression* expr;

    struct Statement* codeBody;
    struct Statement* elseBody;

    struct Statement* next;
} Statement;

/* ============================================== */

typedef struct {
    int type;
    struct Expression* left;
    struct Expression* right;

    char* name;
    int ival;
    char* sval;
} Expression;



//void initAST(AST* ast, char* value);
//void addChild(AST* parent, AST* child);
//void printAST(AST* head);



#endif //AST_H
