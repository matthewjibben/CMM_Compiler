//
// Created by Matthew Jibben on 2/22/2019.
//

#include "ast.h"

void initAST(AST* ast, char* value){
    ast->size = 0;
    ast->value = value;
    ast->left = ast->right = ast->next = NULL;
}

// newAST builds an AST node with the given inputs
AST* newAST(int num, ...){
    // should this be used?
}