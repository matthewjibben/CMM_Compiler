//
// Created by Matthew Jibben on 4/19/2019.
//

#include "IR.h"
#include "ast.h"

extern Program* program;
//global label counters for TAC
int ifelseCount = 0;
int whilecount = 0;

//global string counter. each string must have a unique name in MIPS
int stringCount = 0;    //todo only necessary in MIPS?

/* ========================================== */

Arg* newArg(argType type, int value, char* name){
    Arg* temp = malloc(sizeof(Arg));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    temp->type = type;
    temp->value = value;
    temp->name = name;
    return temp;
}

char* getArgString(Arg* arg){
    char* str;
    if(arg->type==ARG_LABEL){
        char buff[1000];
        snprintf(buff, 1000, "%s%i", arg->name, arg->value);
        str = strdup(buff);
    }
    else if(arg->type==ARG_REGISTER){
        char buff[1000];
        snprintf(buff, 1000, "$%s%i", arg->name, arg->value);
        str = strdup(buff);
    }
    else if(arg->type==ARG_VALUE){
        char buff[1000];
        snprintf(buff, 1000, "%i", arg->value);
        str = strdup(buff);
    }
    else if(arg->type==ARG_VARIABLE || arg->type == ARG_STRING){
        str = arg->name;
    }
    return str;
}

Instruction* newInstruction(instrType type, Arg* arg1, Arg* arg2, Arg* arg3, char* op){
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
            fprintf(output, "\n%s:\n", getArgString(head->arg1));
        }
        else if(head->type == INST_ASSIGN){
            fprintf(output, "%s = %s\n", getArgString(head->arg1), getArgString(head->arg2));
        }
        else if(head->type == INST_ASSIGN_OP){
            fprintf(output, "%s = %s %s %s\n", getArgString(head->arg1), getArgString(head->arg2), head->op, getArgString(head->arg3));
        }
        else if(head->type == INST_COND_JUMP){
            fprintf(output, "%s %s %s\n", head->op, getArgString(head->arg1), getArgString(head->arg2));
        }
        else if(head->type == INST_JUMP){
            fprintf(output, "j %s\n", getArgString(head->arg1));
        }
        else if(head->type == INST_ALLOCATE_ARRAY_INT || head->type == INST_ALLOCATE_ARRAY_VAR){
            fprintf(output, "%s = allocate(%s)\n", getArgString(head->arg1), getArgString(head->arg2));
        }
        else if(head->type == INST_ASSIGN_LW){
            fprintf(output, "lw %s %s(%s)\n", getArgString(head->arg1), getArgString(head->arg2), getArgString(head->arg3));
        }
        else if(head->type == INST_ASSIGN_SW){
            fprintf(output, "sw %s %s(%s)\n", getArgString(head->arg1), getArgString(head->arg2), getArgString(head->arg3));
        }
        head = head->next;
    }
}

Arg* getArrayCellValue(Expression* expr, int c){
    // to get the value in an array cell, we need to use an index and move the address manually
    Arg* index;
    if(expr->type!=CHAR){
        //if needed, multiply index by 4
        Expression* times4 = newExpression(INT, NULL, NULL, NULL, 4, NULL, NULL);
        Expression* multiplication = newExpression(INT, expr->left, times4, NULL, NULL, "*", NULL);
        index = cgen(multiplication, c);
        freeExpression(times4);
        freeExpression(multiplication);
    }
    else {
        index = cgen(expr->left, c);
    }
    // to get an item at index N, we say $t0 = arraypointer + index
    // to use the value, do lw $t1 0($t0)
    // to set a value, do sw $t2 0($t0)
    Arg* cell = newArg(ARG_REGISTER, c, "t");
    Arg* name = newArg(ARG_VARIABLE, 0, expr->name);

    Arg* zero = newArg(ARG_VALUE, 0, NULL);
    Arg* value = newArg(ARG_REGISTER, c+1, "t");
    appendInstruction(program, newInstruction(INST_ASSIGN_OP, cell, index, name, "+"));
    appendInstruction(program, newInstruction(INST_ASSIGN_LW, value, zero, index, NULL));
    //use the lw instruction to get the value inside the cell
    return value;
}

Arg* getArrayCell(Expression* expr, int c){
    // to get the value in an array cell, we need to use an index and move the address manually
    Arg* index;
    if(expr->type!=CHAR){
        //if needed, multiply index by 4
        Expression* times4 = newExpression(INT, NULL, NULL, NULL, 4, NULL, NULL);
        Expression* multiplication = newExpression(INT, expr->left, times4, NULL, NULL, "*", NULL);
        index = cgen(multiplication, c);
        freeExpression(times4);
        freeExpression(multiplication);
    }
    else {
        index = cgen(expr->left, c);
    }
    // to get an item at index N, we say $t0 = arraypointer + index
    // to use the value, do lw $t1 0($t0)
    // to set a value, do sw $t2 0($t0)
    Arg* cell = newArg(ARG_REGISTER, c, "t");
    Arg* name = newArg(ARG_VARIABLE, 0, expr->name);

    appendInstruction(program, newInstruction(INST_ASSIGN_OP, cell, index, name, "+"));
    return cell;
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

Arg* cgen(Expression* expr, int c){
    // first, the tree should be traversed to calculate the weights of each branch. call getBranchWeight() first.

    // if the left and right sides are null, it is a leaf
    // if there is no name, it is a value. if there is a name, it is a var
    if(expr->left==NULL && expr->right==NULL){
        //if it is a value, return the value. otherwise return the string name
        if(expr->name!= NULL){
            return newArg(ARG_VARIABLE, 0, expr->name);
        }
        else if(expr->type==FLOAT) {
            // if the type is float, generate the string and return it
            // if the type is a float, MIPS will need to load into a register and return it
            return newArg(ARG_VALUE, (int)expr->fval, NULL);
        }
        else if(expr->type == INT){
            // if the type is int, generate the string and return it
            return newArg(ARG_VALUE, expr->ival, NULL);
        }
        else if(expr->type == BOOL){
            return newArg(ARG_VALUE, expr->ival, NULL);
        }
        else if(expr->type == STRING){
            return newArg(ARG_STRING, 0, expr->sval);
        }
    }
    if(expr->isArrayCell){
        // if the expresion is an array cell (array name with a given index), it must be used like 0($t0)
        // if the value must be used, set the value on another register using lw $t1 0($t0)
        // if the value must be set, set it using sw $t1 0($t0)
        return getArrayCellValue(expr, c+1);
    }
    if(expr->left!=NULL && expr->right!=NULL){
        Arg* left;
        Arg* right;
        if(expr->type==ASSIGN){
            //if the expression is an assign expression, we can simply assign the left side to the right
            right = cgen(expr->right, c);
            if(expr->left->isArrayCell){
                // if we are assigning a value to the inside of an array, we need to use sw $t1 0($t0)
                left = getArrayCell(expr->left, c+1);
                Arg* zero = newArg(ARG_VALUE, 0, NULL);

                if(right->type==ARG_VALUE){
                    // if the assign value is a pure value, it must be assigned to a register
                    Arg* valueReg = newArg(ARG_REGISTER, c, "t");
                    appendInstruction(program, newInstruction(INST_ASSIGN, valueReg, right, NULL, NULL));
                    //build the instruction and add to the program
                    appendInstruction(program, newInstruction(INST_ASSIGN_SW, valueReg, zero, left, NULL));

                }
                else {
                    //build the instruction and add to the program
                    appendInstruction(program, newInstruction(INST_ASSIGN_SW, right, zero, left, NULL));
                }
            }
            else {
                left = cgen(expr->left, c);
                //build the instruction and add to the program
                appendInstruction(program, newInstruction(INST_ASSIGN, left, right, NULL, NULL));
            }

            //for convention and possible future improvements, the variable name will be returned
            return left;
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
        Arg* arg = newArg(ARG_REGISTER, c, "t");

        appendInstruction(program, newInstruction(INST_ASSIGN_OP, arg, left, right, expr->sval));
        return arg;
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
            Arg* ifLabel = newArg(ARG_LABEL, label, "IF");

            // generate the ELSE label
            Arg* elseLabel = newArg(ARG_LABEL, label, "ELSE");

            // generate the AFTER label
            Arg* afterLabel = newArg(ARG_LABEL, label, "AFTER_IF_ELSE");

            //check the condition
            getBranchWeight(stmt->expr);
            Arg* condition = cgen(stmt->expr, 0);
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
            Arg* ifLabel = newArg(ARG_LABEL, label, "IF");


            // generate the AFTER label
            Arg* afterLabel = newArg(ARG_LABEL, label, "AFTER_IF");


            //check the condition
            getBranchWeight(stmt->expr);
            Arg* condition = cgen(stmt->expr, 0);
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
            Arg* whileLabel = newArg(ARG_LABEL, label, "WHILE");

            // generate the WHILE label
            Arg* afterWhileLabel = newArg(ARG_LABEL, label, "AFTER_WHILE");


            // WHILE label
            appendInstruction(program, newInstruction(INST_LABEL, whileLabel, NULL, NULL, NULL));

            //check the condition
            getBranchWeight(stmt->expr);
            Arg* condition = cgen(stmt->expr, 0);          // todo calculate expression outside loop?
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
            Arg* afterWhileLabel = newArg(ARG_LABEL, label, "AFTER_WHILE");


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
        else if(stmt->type == STMT_DECL){

//            if(stmt->decl->type==FUNCTION){
//
//            }
            if(!stmt->decl->isArray && stmt->decl->value!=NULL){
                //the declaration is of the form "type x = expr"
                //this becomes just a normal assign in the IR
                Arg* value;
                value = cgen(stmt->decl->value, 0);

                Arg* name = newArg(ARG_VARIABLE, 0, stmt->decl->name);

                //build the instruction and add to the program
                appendInstruction(program, newInstruction(INST_ASSIGN, name, value, NULL, NULL));
            }
            else if(stmt->decl->isArray){
                //need to allocate the necessary size in the heap
                //if it is a character array, that is the size in bytes (each character is one byte)
                //any other type of array is the size * 4, since we use 32 bit integers or addresses
                int size = stmt->decl->size;
                if(stmt->type != CHAR){
                    size *= 4;
                }
                //syscall 9 can be used to allocate in the heap, however this will only be done in MIPS
                //in the IR, it is more compact to use the form "var = allocate(bytesize)"
                if(stmt->decl->value==NULL) {
                    //the array is allocated with a integer
                    Arg* sizearg = newArg(ARG_VALUE, size, NULL);
                    Arg* name = newArg(ARG_VARIABLE, 0, stmt->decl->name);
                    appendInstruction(program,
                                      newInstruction(INST_ALLOCATE_ARRAY_INT, name, sizearg, NULL, NULL));
                } else {
                    Arg* name = newArg(ARG_VARIABLE, 0, stmt->decl->name);
                    Arg* varname = newArg(ARG_VARIABLE, 0, stmt->decl->value->name);
                    //the array is allocated using a variable
                    appendInstruction(program,
                                      newInstruction(INST_ALLOCATE_ARRAY_VAR, name, varname, NULL, NULL));

                }
            }
        }
        stmt = stmt->next;
    }
}