//
// Created by Matthew Jibben on 4/19/2019.
//

#include "IR.h"
#include "ast.h"

extern FILE* outputTAC;


int getBranchWeight(Expression* expr){
    //recursively get the weight of each branch to calculate the weights for the tree
    // this will be used in translation to minimize the number of temp registers needed via Sethi and Ullman's algorithm
    int leftWeight = 0;
    int rightWeight = 0;
    if(expr->left==NULL && expr->right==NULL){
        expr->weight = 1;
//        printf("branch leaf: 1\n");
        return 1;   // this branch is a leaf, return 1
    }
    if(expr->left!=NULL){
        leftWeight = getBranchWeight(expr->left);
    }
    if(expr->right!=NULL){
        rightWeight = getBranchWeight(expr->right);
    }
    expr->weight = leftWeight + rightWeight;
//    printf("branch weight: %i\n", expr->weight);
    return expr->weight;
}

char* cgen(Expression* expr, int c){
    // first, the tree should be traversed to calculate the weights of each branch. call getBranchWeight() first.

    // if the left and right sides are null, it is a leaf
    // if there is no name, it is a value. if there is a name, it is a var
    if(expr->left==NULL && expr->right==NULL){
        //if it is a value, return the value. otherwise return the string name
        if(expr->name!= NULL){
            return expr->name;
        }
        else if(expr->type==FLOAT) {
            // if the type is float, generate the string and return it
            // if the type is a float, MIPS will need to load into a register and return it

            char buff[100];
            snprintf(buff, 100, "%i", (int)expr->fval);
            char* str = buff;
            return strdup(str);
        }
        else if(expr->type == INT){
            // if the type is int, generate the string and return it
//            fprintf(outputTAC, "$t%i = %i\n", c, expr->ival); // This will be necessary for
            char buff[100];
            snprintf(buff, 100, "%i", expr->ival);
            char* str = buff;
            return strdup(str);
        }
    }
    if(expr->left!=NULL && expr->right!=NULL){
        char* left;
        char* right;
        if(expr->type==ASSIGN){
            //if the expression is an assign expression, we can simply assign the left side to the right
            right = cgen(expr->right, c);
            fprintf(outputTAC, "%s = %s\n", expr->left->name, right);   // todo should be a builder function

            //for convention and possible future improvements, the variable name will be returned
            return expr->left->name;
        }
        //if there are expressions on both sides, it is a full operation like "a OP b"
        //the order of translation should be based on the weight of each branch, (Sethi and Ullman's algorithm)
        //this minimizes the number of required temporary registers

        if(expr->left->weight >= expr->right->weight){
            left = cgen(expr->left, c);
            right = cgen(expr->right, c+1);
        }
        else{
            right = cgen(expr->right, c);
            left = cgen(expr->left, c+1);
        }
        //if this expression is a float, use $fX = expression
        fprintf(outputTAC, "$t%i = %s %s %s\n", c, left, expr->sval, right);        // todo should be a builder function

        //return the register that was used
        char buff[5];
        snprintf(buff, 5, "$t%i", c);
        char* str = buff;
        return strdup(str);
    }
}

char* cgenStatement(Statement* stmt){

}