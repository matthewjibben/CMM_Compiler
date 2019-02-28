//
// Created by Matthew Jibben on 2/22/2019.
//

#include "ast.h"

//void initAST(AST* ast, char* value){
//    ast->size = 0;
//    ast->value = value;
//    ast->left = ast->right = ast->next = NULL;
//}
//
//// newAST builds an AST node with the given inputs
//AST* newAST(int num, ...){
//    // should this be used?
//}


Declaration* newDeclaration(char* name, bool isArray, int type, int returnType,
        Expression* value, Statement* codeBlock, paramList* params, Declaration* next){
    Declaration *temp = malloc(sizeof(Declaration));
    temp->name = name;
    temp->isArray = isArray;
    temp->type = type;
    temp->returnType = returnType;
    temp->value = value;
    temp->codeBlock = codeBlock;
    temp->params = params;
    temp->next= next;
    return temp;
}

Statement* newStatement(stmtType type, Declaration* decl, Expression* expr,
        Statement* codeBody, Statement* elseBody, Statement* next){
    Statement *temp = malloc(sizeof(Statement));
    temp->type = type;
    temp->decl = decl;
    temp->expr = expr;
    temp->codeBody = codeBody;
    temp->elseBody = elseBody;
    temp->next = next;
    return temp;
}

Expression* newExpression(int type, Expression* left, Expression* right, char* name, int ival, char* sval){
    Expression *temp = malloc(sizeof(Expression));
    temp->type = type;
    temp->left = left;
    temp->right= right;
    temp->name = name;
    temp->ival = ival;
    temp->sval = sval;
    return temp;
}

void printExpression(Expression* expr){
    if(expr!=NULL){
        printf("\n/==============\\\n");
        if(expr->ival!=NULL)
            printf("ival: %i ", expr->ival);
        if(expr->sval!=NULL)
            printf("sval: %s ", expr->sval);
        if(expr->name!=NULL)
            printf("name: %s ", expr->name);

        if(expr->left!=NULL) {
            printf("\n");
            printExpression(expr->left);
        }
        if(expr->right!=NULL) {
            printf("\n");
            printExpression(expr->right);
        }
        printf("\n\\==============/\n");
    }
}

void printDeclaration(Declaration* decl){

}

void printStatement(Statement* stmt){

}
//todo treefree function is needed