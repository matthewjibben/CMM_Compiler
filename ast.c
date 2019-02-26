//
// Created by Matthew Jibben on 2/22/2019.
//

#include "ast.h"

void initAST(AST* ast, char* value){
    ast->size = 0;
    ast->value = value;
    ast->childHead = ast->childTail = ast->next = ast->prev = NULL;
}

