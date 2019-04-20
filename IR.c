//
// Created by Matthew Jibben on 4/19/2019.
//

#include "IR.h"
#include "ast.h"

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

char* cgen(Expression* expr){
    // first, the tree should be traversed to calculate the weights of each branch

    // if the left and right sides are null, it is a leaf
    // if there is no name, it is a value. if there is a name, it is a var
    if(expr->left==NULL && expr->right==NULL){
        //if it is a value, return the value. otherwise return the string name
        if(expr->name!= NULL){
            return expr->name;
        }
        else if(expr->type==FLOAT) {
            // if the type is float, generate the string and return it
            char buff[100];
            snprintf(buff, 100, "%f", expr->fval);
            char* str = buff;
            return strdup(str);
        }
        else if(expr->type == INT){
            // if the type is int, generate the string and return it
            char buff[100];
            snprintf(buff, 100, "%i", expr->ival);
            char* str = buff;
            return strdup(str);
        }
    }
    if(expr->left!=NULL && expr->right!=NULL){
        //if there are expressions on both sides, it is a full operation like "a OP b"
        printf("_tc = %s %s %s\n", cgen(expr->left), expr->sval, cgen(expr->right));
        return "_tc";
    }
}