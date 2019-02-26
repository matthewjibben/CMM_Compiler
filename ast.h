//
// Created by Matthew Jibben on 2/22/2019.
//

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>


typedef struct {
    int size;
    char* value;

    // pointers to the linked list of child nodes
    // having a linked list of children allows for any number of children
    struct AST* childHead;
    struct AST* childTail;

    // pointers to the nodes on the same level (siblings)
    struct AST* next;
    struct AST* prev;
} AST;


typedef struct {
    AST* head;
} ASTHead;

void initAST(AST* ast, char* value);
void addChild(AST* parent, AST* child);
void printAST(AST* head);



#endif //AST_H
