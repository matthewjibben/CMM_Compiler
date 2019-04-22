//
// Created by Matthew Jibben on 4/19/2019.
//

#include "IR.h"
#include "ast.h"

extern Program* program;
//global label counters for TAC
int ifelseCount = 0;
int whilecount = 0;

/* ========================================== */

Instruction* newInstruction(instrType type, char* arg1, char* arg2, char* arg3, char* op){
    Instruction* temp = malloc(sizeof(Instruction));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->type = type;
    temp->arg1 = arg1;
    temp->arg2 = arg2;
    temp->arg3 = arg3;
    temp->op = op;
    return temp;
}

void appendInstruction(Program* prog, Instruction* instr){
    if(prog->head == NULL){
        //if this is the first instruction, set it to the head and tail
        prog->head = prog->tail = instr;
    } else {
        prog->tail->next = instr;
        prog->tail = prog->tail->next;
    }
}

void emit(Program* prog, FILE* output){
    Instruction* head = prog->head;
    while(head!=NULL){
        if(head->type==INST_LABEL){
            fprintf(output, "\n%s:\n", head->arg1);
        }
        else if(head->type == INST_ASSIGN){
            fprintf(output, "%s = %s\n", head->arg1, head->arg2);
        }
        else if(head->type == INST_ASSIGN_OP){
            fprintf(output, "%s = %s %s %s\n", head->arg1, head->arg2, head->op, head->arg3);
        }
        else if(head->type == INST_COND_JUMP){
            fprintf(output, "%s %s %s\n", head->op, head->arg1, head->arg2);
        }
        else if(head->type == INST_JUMP){
            fprintf(output, "j %s\n", head->arg1);
        }
        head = head->next;
    }
}


/* ========================================== */
int getBranchWeight(Expression* expr){
    //recursively get the weight of each branch to calculate the weights for the tree
    // this will be used in translation to minimize the number of temp registers needed via Sethi and Ullman's algorithm
    int leftWeight = 0;
    int rightWeight = 0;
    if(expr->left==NULL && expr->right==NULL){
        expr->weight = 1;
        return 1;   // this branch is a leaf, return 1
    }
    if(expr->left!=NULL){
        leftWeight = getBranchWeight(expr->left);
    }
    if(expr->right!=NULL){
        rightWeight = getBranchWeight(expr->right);
    }
    expr->weight = leftWeight + rightWeight;
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
        else if(expr->type == BOOL){
            char buff[2];
            snprintf(buff, 2, "%i", expr->ival);
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

            //build the instruction and add to the program
            appendInstruction(program, newInstruction(INST_ASSIGN, expr->left->name, right, NULL, NULL));
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
        //if this expression is a float, use $fX = expression? future implementation
        //return the register that was used
        char buff[5];
        snprintf(buff, 5, "$t%i", c);
        char* str = strdup(buff);

        appendInstruction(program, newInstruction(INST_ASSIGN_OP, str, left, right, expr->sval));
        return str;
    }
}

char* cgenStatement(Statement* stmt){
    while(stmt!=NULL) {
        if(stmt->type == STMT_IF_ELSE) {
            //for if else statements, generate a new label for each block using the counter
            int label = ifelseCount;
            ifelseCount++;
            //generate the necessary labels
            // generate the IF label
            char buff[1000];
            snprintf(buff, 1000, "IF%i", label);
            char *ifLabel = strdup(buff);

            // generate the ELSE label
            snprintf(buff, 1000, "ELSE%i", label);
            char *elseLabel = strdup(buff);

            // generate the AFTER label
            snprintf(buff, 1000, "AFTER_IF_ELSE%i", label);
            char *afterLabel = strdup(buff);

            //check the condition
            getBranchWeight(stmt->expr);
            char* condition = cgen(stmt->expr, 0);
            //if the condition is false (0), jump to the else label. otherwise continue like normal to the if label
            appendInstruction(program, newInstruction(INST_COND_JUMP, condition, elseLabel, NULL, "beqz"));


            // IF label
            appendInstruction(program, newInstruction(INST_LABEL, ifLabel, NULL, NULL, NULL));
            //run cgen on the IF code body
            cgenStatement(stmt->codeBody);
            //jump to the end
            appendInstruction(program, newInstruction(INST_JUMP, afterLabel, NULL, NULL, NULL));


            // ELSE label
            appendInstruction(program, newInstruction(INST_LABEL, elseLabel, NULL, NULL, NULL));
            //run cgen on the ELSE code body
            cgenStatement(stmt->elseBody);


            // END label
            appendInstruction(program, newInstruction(INST_LABEL, afterLabel, NULL, NULL, NULL));
        }
        else if(stmt->type == STMT_IF){
            //for if statements, generate a new label for each block using the counter
            int label = ifelseCount;
            ifelseCount++;
            //generate the necessary labels
            // generate the IF label
            char buff[1000];
            snprintf(buff, 1000, "IF%i", label);
            char *ifLabel = strdup(buff);

            // generate the AFTER label
            snprintf(buff, 1000, "AFTER_IF%i", label);
            char *afterLabel = strdup(buff);

            //check the condition
            getBranchWeight(stmt->expr);
            char* condition = cgen(stmt->expr, 0);
            //if the condition is false (0), jump to the else label. otherwise continue like normal to the if label
            appendInstruction(program, newInstruction(INST_COND_JUMP, condition, afterLabel, NULL, "beqz"));

            // IF label
            appendInstruction(program, newInstruction(INST_LABEL, ifLabel, NULL, NULL, NULL));
            //run cgen on the IF code body
            cgenStatement(stmt->codeBody);

            // END label
            appendInstruction(program, newInstruction(INST_LABEL, afterLabel, NULL, NULL, NULL));
        }
        else if(stmt->type == STMT_WHILE){
            //generate a new label count for the while block
            int label = whilecount;
            whilecount++;
            //generate the necessary labels
            // generate the WHILE label
            char buff[1000];
            snprintf(buff, 1000, "WHILE%i", label);
            char *whileLabel = strdup(buff);
            // generate the WHILE label
            snprintf(buff, 1000, "AFTER_WHILE%i", label);
            char *afterWhileLabel = strdup(buff);

            // WHILE label
            appendInstruction(program, newInstruction(INST_LABEL, whileLabel, NULL, NULL, NULL));

            //check the condition
            getBranchWeight(stmt->expr);
            char* condition = cgen(stmt->expr, 0);          // todo calculate expression outside loop?
            //if the condition is false (0), jump to the after label. otherwise continue through the code block
            appendInstruction(program, newInstruction(INST_COND_JUMP, condition, afterWhileLabel, NULL, "beqz"));

            //run cgen on the WHILE code body
            cgenStatement(stmt->codeBody);

            //unconditional jump back to loop back to the start of the while loop
            appendInstruction(program, newInstruction(INST_JUMP, whileLabel, NULL, NULL, NULL));


            // AFTER WHILE label
            appendInstruction(program, newInstruction(INST_LABEL, afterWhileLabel, NULL, NULL, NULL));
        }
        else if(stmt->type == STMT_BREAK){
            //for a break statement, we need to find the current after_loop label and jump to it
            //because there are only while loops currently, jump to AFTER_WHILEi
            //get the necessary label
            int label = whilecount-1;
            char buff[1000];
            snprintf(buff, 1000, "AFTER_WHILE%i", label);
            char *afterWhileLabel = strdup(buff);

            //jump to the end of the while loop, breaking the loop
            appendInstruction(program, newInstruction(INST_JUMP, afterWhileLabel, NULL, NULL, NULL));
        }
        else if(stmt->type == STMT_EXPR){
            //todo check if expression is necessary (only function calls and assigns are necessary)
            getBranchWeight(stmt->expr);
            cgen(stmt->expr, 0);
        }
        else if(stmt->type == STMT_BLOCK){
            cgenStatement(stmt->codeBody);
        }
        stmt = stmt->next;
    }
}