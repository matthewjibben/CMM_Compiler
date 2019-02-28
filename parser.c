//
// Created by Matthew Jibben on 2/5/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include "cmm.tab.h"

extern int yyparse();
extern FILE* yyin;

int main(int argc, char **argv)
{
    printf("=========================parser.c===============================================\n");
    //open the input sample cmm file
    if((yyin = fopen("./sample.cmm","r"))==NULL){
        printf("Failed to read file");
        exit(1);
    }
    else {
        printf("file correctly opened\n");
    }

    //int c;
    //while((c=getc(yyin))!=EOF){
    //    putchar(c);
    //}


    printf("\n\nyyparse: %i\n", yyparse());


//    printf("\n==============================AST TESTING==============================\n");
//    Declaration* myDec  = newDeclaration("mynameSADFASD", false, INT, NULL, NULL, NULL, NULL, NULL);
//    Declaration* myDec2 = newDeclaration("myname2", false, STRING, NULL, NULL, NULL, NULL, myDec);
//    Expression* myExpr = newExpression(INT, NULL, NULL, NULL, 10, NULL);
//    Expression* myExpr2 = newExpression(INT, NULL, NULL, NULL, 10, NULL);
//    Expression* LE_Expr = newExpression(LE, myExpr, myExpr2, NULL, NULL, "<=");
//
//    printf("Expression: root: %s    Left:  %i     Right: %i \n", LE_Expr->sval, LE_Expr->left->ival, LE_Expr->right->ival);
//
//    Statement* myStmt = newStatement(STMT_IF, myDec2, LE_Expr, NULL, NULL, NULL);
//    printf("Statement: type %i, decl: %s, expr: %s", myStmt->type, myStmt->decl->name, myStmt->expr->sval);
//
////    printf(myDec2->next->name);
////    printf("\n type: %i", myDec->type);
//
//
//    printf("\n==============================AST TESTING==============================\n");
    fclose(yyin);
    printf("exit\n\n");
    return 0;
}
