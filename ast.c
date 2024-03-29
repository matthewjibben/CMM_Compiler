//
// Created by Matthew Jibben on 2/22/2019.
//

#include <string.h>
#include <stdlib.h>
#include "ast.h"


Declaration* newDeclaration(char* name, bool isArray, int type, int returnType, int size,
        Expression* value, Statement* codeBlock, ParamList* params){
    Declaration* temp = malloc(sizeof(Declaration));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
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
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
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
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->type = type;
    temp->left = left;
    temp->right= right;
    temp->name = name;
    temp->ival = ival;
    temp->sval = sval;
    temp->args = args;
    return temp;
}

/* new expression struct builder used purely for float values */
Expression* newExpressionFloat(int type, Expression* left, Expression* right, char* name, float fval, char* sval,
                               ArgList* args){
    Expression* temp = malloc(sizeof(Expression));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->type = type;
    temp->left = left;
    temp->right= right;
    temp->name = name;
    temp->fval = fval;
    temp->sval = sval;
    temp->args = args;
    return temp;
}

/* ================= */

Param* newParam(int type, char* name, Param* next, bool isArray){
    Param* temp = malloc(sizeof(Param));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->type = type;
    temp->name = name;
    temp->next = next;
    temp->isArray = isArray;
    return temp;
}

ParamList* newParamList(Param* param){
    ParamList* temp = malloc(sizeof(ParamList));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->head = temp->tail = param;
    temp->size = 0;
    if(temp->head != NULL){
        temp->size++;
    }
    return temp;
}

void appendParam(ParamList* paramList, Param* param){
    //append a parameter to the end of the linked list
    //if the list is empty, set a new head and tail
    if(paramList != NULL) {
        if(paramList->head!=NULL) {
            paramList->tail->next = param;
            paramList->tail = paramList->tail->next;
        }
        else {
            paramList->head = paramList->tail = param;
        }
        paramList->size++;
//        printf("paramlist size: %i", paramList->size);
    }
    else{
        printf("paramList list is null! this should not happen!");
    }
}

/* ================= */


ArgList* newArgList(Argument* arg){
    ArgList* temp = malloc(sizeof(ArgList));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->head = temp->tail = arg;
    temp->size = 0;
    if(temp->head != NULL){
        temp->size++;
    }
    return temp;
}

Argument* newArgument(Argument* next, Expression* expr){
    Argument* temp = malloc(sizeof(Argument));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->next = next;
    temp->expr = expr;
    return temp;
}

void appendArgument(ArgList* argList, Argument* arg){
    if(argList != NULL) {
        if(argList->head!=NULL) {
            argList->tail->next = arg;
            argList->tail = argList->tail->next;
        } else {
            argList->head = argList->tail = arg;
        }
        argList->size++;
    }
    else {
        printf("argList list is null! this should not happen!");
    }
}

/* ================= */

StatementList* newStatementList(Statement* stmt){
    StatementList* temp = malloc(sizeof(StatementList));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->head = temp->tail = stmt;
    return temp;
}

void appendStatement(StatementList* stmtList, Statement* stmt){
    if(stmtList != NULL) {
        if(stmtList->head!=NULL) {
            stmtList->tail->next = stmt;
            stmtList->tail = stmtList->tail->next;
        } else {
            stmtList->head = stmtList->tail = stmt;

        }
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
        printf("type: ");
        printTypeString(expr->type);
        printf(" ");
        //create negation string
        //this is only used when an int or float is negated with unary "-"
        char* negationString;
        if(expr->isUnaryNegate)
            negationString = "-";
        else
            negationString = "";

        //print if the expression has been given a NOT
        if(expr->isUnaryNot){
            printf("NOT ");
        }

        //only  print the ival if it is set and it is not an operation such as "+"
        if(expr->ival!=NULL || expr->type==INT && expr->sval==NULL) {
            printf("ival: %s%i ", negationString, expr->ival);
        }
        if(expr->type==FLOAT) {
            printf("fval: %s%f ", negationString, expr->fval);
        }
        if(expr->type==BOOL) {
            if(expr->ival)
                printf("boolean: true ");
            else
                printf("boolean: false ");
        }
        if(expr->sval!=NULL)
            printf("sval: %s ", expr->sval);
        if(expr->name!=NULL) {
            printf("name: %s", expr->name);
            if(expr->isFunctionCall)
                printf("()");
            printf(" ");
        }
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
    Argument* temp = args->head;
    while(temp!=NULL){
        printExpression(temp->expr, indent);
        temp = temp->next;
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
        if(decl->name!=NULL) {
            printIndent(indent);
            printf("type: ");
            printTypeString(decl->type);
            printf(" name: %s", decl->name);
            if(decl->isArray) {
                printf("[%i]", decl->size);
                if(decl->size!=NULL) {
                    printf(" size: %i", decl->size);
                }
            } else if(decl->type==FUNCTION){
                printParams(decl->params);
                printf("return: ");
                printTypeString(decl->returnType);
                printf("\n\n");
                printIndent(indent);
                printf("Enter code block: \n");
                printStatement(decl->codeBlock, indent+1);
                printIndent(indent);
                printf("Exit code block \n");
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
        if(stmt->type==STMT_NULL) {
            printIndent(indent-1);
            printf("No Operation\n");
        }
        else if(stmt->type==STMT_EXPR){
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
    else if(typeToken==FLOAT){
        printf("float");
    }
    else if(typeToken==FUNCTION){
        printf("function");
    }
    else if(typeToken==BOOL){
        printf("boolean");
    }
}
//same as printtypestring, but returns the string
char* getTypeString(int typeToken){
    if(typeToken==INT){
        return "int";
    }
    else if(typeToken==CHAR){
        return "char";
    }
    else if(typeToken==STRING){
        return "string";
    }
    else if(typeToken==FLOAT){
        return "float";
    }
    else if(typeToken==FUNCTION){
        return "function";
    }
    else if(typeToken==BOOL){
        return "boolean";
    }
    else if(typeToken==ASSIGN){
        return "assign";
    }
    else {
        return "\"other type\"";
    }
}

/* ======================================= */

void freeStmtList(StatementList* stmtList){
    free(stmtList);
}


void freeExpression(Expression* expr){
    if(expr!=NULL){
        freeExpression(expr->left);
        freeExpression(expr->right);
        freeArgList(expr->args);
        free(expr);
    }
}

void freeDeclaration(Declaration* decl){
    if(decl!=NULL){
        freeExpression(decl->value);
        freeStatement(decl->codeBlock);
        freeParamList(decl->params);
        free(decl);
    }
}

void freeStatement(Statement* stmt){
    if(stmt!=NULL){
        freeDeclaration(stmt->decl);
        freeExpression(stmt->expr);
        freeStatement(stmt->codeBody);
        freeStatement(stmt->elseBody);
        freeStatement(stmt->next);
        free(stmt);
    }
}

void freeArgList(ArgList* argList){
    if(argList!=NULL){
        freeArgument(argList->head);
        free(argList);
    }
}

void freeArgument(Argument* arg){
    if(arg!=NULL){
        freeArgument(arg->next);
        freeExpression(arg->expr);
        free(arg);
    }
}

void freeParamList(ParamList* params){
    if(params!=NULL){
        freeParameter(params->head);
        free(params);
    }
}

void freeParameter(Param* param){
    if(param!=NULL){
        free(param->name);
        free(param->next);
        free(param);
    }
}