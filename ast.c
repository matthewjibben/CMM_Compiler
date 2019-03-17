//
// Created by Matthew Jibben on 2/22/2019.
//

#include <string.h>
#include <stdlib.h>
#include "ast.h"


Declaration* newDeclaration(char* name, bool isArray, int type, int returnType, int size,
        Expression* value, Statement* codeBlock, ParamList* params){
    Declaration* temp = malloc(sizeof(Declaration));
    temp->name = name;
    temp->isArray = isArray;
    temp->type = type;
    temp->returnType = returnType;
    temp->size = size;
    temp->value = value;
    temp->codeBlock = codeBlock;
    temp->params = params;
    //temp->next= next;
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
    temp->args = args;
    return temp;
}

/* ================= */

Param* newParam(int type, char* name, Param* next, bool isArray){
    Param* temp = malloc(sizeof(Param));
    temp->type = type;
    temp->name = name;
    temp->next = next;
    temp->isArray = isArray;
    return temp;
}

ParamList* newParamList(Param* param){
    ParamList* temp = malloc(sizeof(ParamList));
    temp->head = temp->tail = param;
    return temp;
}

void appendParam(ParamList* paramList, Param* param){
    if(paramList != NULL) {
        paramList->tail->next = param;
        paramList->tail = paramList->tail->next;
    }
    else{
        printf("paramList list is null! this should not happen!");
    }
}

/* ================= */

ArgList* newArgList(ArgList* next, Expression* expr){
    ArgList* temp = malloc(sizeof(ArgList));
    temp->next = next;
    temp->expr = expr;
    return temp;
}

/* ================= */

StatementList* newStatementList(Statement* stmt){
    StatementList* temp = malloc(sizeof(StatementList));
    temp->head = temp->tail = stmt;
    return temp;
}

void appendStatement(StatementList* stmtList, Statement* stmt){
    if(stmtList != NULL) {
        stmtList->tail->next = stmt;
        stmtList->tail = stmtList->tail->next;
    }
    else{
        printf("statement list is null! this should not happen!");
    }
}


/* ================= */


void printStatementList(StatementList* stmtList, int indent){
    if(stmtList!=NULL) {
        Statement *temp = stmtList->head;
        while (temp != NULL) {
            printf("printing...\n");
            printStatement(temp, indent);
            temp = temp->next;
        }
    }
}


void printExpression(Expression* expr, int indent){
    if(expr!=NULL){
//        printf("/========|");
//        for(int i=0; i<indent; ++i){
//            printf("/========|");
//        }
        printIndent(indent);
        if(expr->ival!=NULL || expr->type==INT)
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
    }
}

void printArgList(ArgList* args, int indent){
    printf("\n");
    printIndent(indent);
    printf("**Args**\n");
    while(args!=NULL){
        printExpression(args->expr, indent);
        args = args->next;
    }
    printIndent(indent);
    printf("********\n");
}
void printIndent(int indent){
    for(int i=0; i<indent+1; ++i){
        printf("/========|");
    }
}
void printDeclaration(Declaration* decl, int indent){
    if(decl!=NULL){
        printIndent(indent-1);
        printf("Declaration:\n\n");
//        for(int i=0; i<indent; ++i){
//            printf("/========|");
//        }
        if(decl->name!=NULL) {
            printIndent(indent);
            printf("name: %s", decl->name);
            if(decl->isArray) {
                printf("[%i]", decl->size);
                if(decl->size!=NULL) {
                    printf(" size: %i", decl->size);
                }
            } else if(decl->type==FUNCTION){
                printParams(decl->params);
                printf("\n");
                printStatement(decl->codeBlock, indent+1);
            }
            printf(" \n");
        }

        if(decl->value!=NULL){
            printExpression(decl->value, indent);
        }
        printf("\n");
        printIndent(indent-1);
        printf("End declaration\n");
    }
}

void printParams(ParamList* params){
    printf("(");
    if(params!=NULL) {
        Param *temp = params->head;
        while (temp != NULL){
            printTypeString(temp->type);
            printf(" %s", temp->name);
            if(temp->isArray){
                printf("[]");
            }
            temp = temp->next;
            if (temp != NULL) {
                printf(",");
            }
        }
    }
    printf(") ");
}

void printStatement(Statement* stmt, int indent){
    if(stmt!=NULL){
        // print statements for every type of statement
        if(stmt->type==STMT_EXPR){
            printIndent(indent-1);
            printf("Expression statement: \n");
            printExpression(stmt->expr, indent);
        }
        else if(stmt->type==STMT_DECL) {
            printDeclaration(stmt->decl, indent);
        }
        else if(stmt->type==STMT_READ) {
            printIndent(indent-1);
            printf("READ: \n");
            printExpression(stmt->expr, indent);
        }
        else if(stmt->type==STMT_WRITE) {
            printIndent(indent-1);
            printf("WRITE: \n");
            printExpression(stmt->expr, indent);
        }
        else if(stmt->type==STMT_WRITELN) {
            printIndent(indent-1);
            printf("WRITELN \n");
        }
        else if(stmt->type==STMT_BREAK) {
            printIndent(indent-1);
            printf("BREAK \n");
        }
        else if(stmt->type==STMT_BLOCK) {
            printIndent(indent-1);
            printf("BLOCK: \n\n");
            printStatement(stmt->codeBody, indent+1);
            printIndent(indent-1);
            printf("END BLOCK \n");
        }
        else if(stmt->type==STMT_RETRN) {
            printIndent(indent-1);
            printf("RETURN: \n");
            printExpression(stmt->expr, indent);
        }
        else if(stmt->type==STMT_WHILE) {
            printIndent(indent-1);
            printf("WHILE: \n\n");
            printExpression(stmt->expr, indent);

            printIndent(indent-1);
            printf("RUN: \n\n");

            printStatement(stmt->codeBody, indent+1);
            printIndent(indent-1);
            printf("END WHILE\n");
        }
        else if(stmt->type==STMT_IF) {
            printIndent(indent-1);
            printf("IF: \n\n");
            printExpression(stmt->expr, indent);

            printIndent(indent-1);
            printf("RUN: \n\n");

            printStatement(stmt->codeBody, indent+1);
            printIndent(indent-1);
            printf("END IF\n");
        }
        else if(stmt->type==STMT_IF_ELSE) {
            printIndent(indent-1);
            printf("IF: \n\n");
            printExpression(stmt->expr, indent);

            printIndent(indent-1);
            printf("RUN: \n\n");
            printStatement(stmt->codeBody, indent+1);

            printIndent(indent-1);
            printf("ELSE RUN: \n\n");
            printStatement(stmt->elseBody, indent+1);

            printIndent(indent-1);
            printf("END IF_ELSE\n");
        }

        printf("\n");

        stmt = stmt->next;
        if(stmt!=NULL) {
            printIndent(indent-1);
            printf("Next Statement ------------------------------>\n\n");
        }
        printStatement(stmt, indent);
    }
}

void printTypeString(int typeToken){
    if(typeToken==INT){
        printf("int");
    }
    else if(typeToken==CHAR){
        printf("char");
    }
    else if(typeToken==STRING){
        printf("string");
    }
}

/* ======================================= */

void freeStmtList(StatementList* stmtList){
    free(stmtList);
}

//todo treefree function is needed