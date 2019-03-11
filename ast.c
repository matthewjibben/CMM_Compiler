//
// Created by Matthew Jibben on 2/22/2019.
//

#include "ast.h"


Declaration* newDeclaration(char* name, bool isArray, int type, int returnType, int size,
        Expression* value, Statement* codeBlock, ParamList* params, Declaration* next){
    Declaration* temp = malloc(sizeof(Declaration));
    temp->name = name;
    temp->isArray = isArray;
    temp->type = type;
    temp->returnType = returnType;
    temp->size = size;
    temp->value = value;
    temp->codeBlock = codeBlock;
    temp->params = params;
    temp->next= next;
    return temp;
}

Statement* newStatement(stmtType type, Declaration* decl, Expression* expr,
        Statement* codeBody, Statement* elseBody, Statement* next){
    Statement* temp = malloc(sizeof(Statement));
    temp->type = type;
    temp->decl = decl;
    temp->expr = expr;
    temp->codeBody = codeBody;
    temp->elseBody = elseBody;
    temp->next = next;
    return temp;
}

Expression* newExpression(int type, Expression* left, Expression* right, char* name, int ival, char* sval,
        ArgList* args){
    Expression* temp = malloc(sizeof(Expression));
    temp->type = type;
    temp->left = left;
    temp->right= right;
    temp->name = name;
    temp->ival = ival;
    temp->sval = sval;
    temp->args= args;
    return temp;
}

ParamList* newParamList(ParamList* next, int type, char* name){
    ParamList* temp = malloc(sizeof(ParamList));
    temp->next = next;
    temp->type = type;
    temp->name = name;
    return temp;
}

ArgList* newArgList(ArgList* next, Expression* expr){
    ArgList* temp = malloc(sizeof(ArgList));
    temp->next = next;
    temp->expr = expr;
    return temp;
}

/* ================= */

void appendStatement(StatementList* stmtList, Statement* stmt){
    if(stmtList != NULL) {
        stmtList->tail->next = stmt;
        stmtList->tail = stmtList->tail->next;
    }
    else{
        printf("statement list is null! this should not happen");
    }
}

/* ================= */




void printExpression(Expression* expr, int indent){
    if(expr!=NULL){
        printf("/========|");
        for(int i=0; i<indent; ++i){
            printf("/========|");
        }
        if(expr->ival!=NULL)
            printf("ival: %i ", expr->ival);
        if(expr->sval!=NULL)
            printf("sval: %s ", expr->sval);
        if(expr->name!=NULL)
            printf("name: %s ", expr->name);
        if(expr->args!=NULL)
            printArgList(expr->args, indent+1);
        printf("\n");
        if(expr->left!=NULL) {
            printExpression(expr->left, indent+1);
        }
        if(expr->right!=NULL) {
            printExpression(expr->right, indent+1);
        }
//        printf("\n\\==============/\n");
    }
    //printf("/========|");
}

void printArgList(ArgList* args, int indent){
    printf("\n**Args**\n");
    while(args!=NULL){
        printExpression(args->expr, indent);
        args = args->next;
    }
    printf("********\n");
}

void printDeclaration(Declaration* decl){

}

void printStatement(Statement* stmt){

}
//todo treefree function is needed