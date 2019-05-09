//
// Created by Matthew Jibben on 4/19/2019.
//

#include "IR.h"
#include "ast.h"
#include "symboltable.h"

extern Program* program;
extern Env* env;
//global label counters for TAC
int ifelseCount = 0;
int whilecount = 0;

//global string counter. each string must have a unique name in MIPS
int stringCount = 0;    //todo only necessary in MIPS?

char* currentFunctionName;      // used to help with return statements
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
    else if(arg->type==ARG_VARIABLE || arg->type == ARG_STRING || arg->type == ARG_CHAR){
        str = arg->name;
    }
    else if(arg->type == ARG_SP){
        return "$sp";
    }
    else if(arg->type == ARG_RA){
        return "$ra";
    }
    else if(arg->type == ARG_ZERO){
        return "$zero";
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
        instr->prev = prog->tail;
        prog->tail = prog->tail->next;
    }
    //emitPrintStatement(instr);
}

void removeInstruction(Instruction* instr, Program* program){
    if(instr->prev!=NULL){
        instr->prev->next = instr->next;
    }
    else {
        // instruction is the head of program, move head
        program->head = program->head->next;
    }
    if(instr->next!=NULL){
        instr->next->prev = instr->prev;
    }
    else {
        // instruction is the tail of program, move tail
        program->tail = program->tail->next;
    }
    free(instr);
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
        else if(head->type == INST_WRITE_INT || head->type == INST_WRITE_STR){
            fprintf(output, "write %s\n", getArgString(head->arg1));
        }
        else if(head->type == INST_READ){
            fprintf(output, "read\n");
        }
        else if(head->type == INST_FUNCCALL){
            fprintf(output, "functioncall %s\n", getArgString(head->arg1));
        }
        head = head->next;
    }
}

void emitPrintStatement(Instruction* instruction){
    Instruction* head = instruction;
//    while(head!=NULL){
        if(head->type==INST_LABEL){
            printf("\n%s:\n", getArgString(head->arg1));
        }
        else if(head->type == INST_ASSIGN){
            printf("%s = %s\n", getArgString(head->arg1), getArgString(head->arg2));
        }
        else if(head->type == INST_ASSIGN_OP){
            printf("%s = %s %s %s\n", getArgString(head->arg1), getArgString(head->arg2), head->op, getArgString(head->arg3));
        }
        else if(head->type == INST_COND_JUMP){
            printf("%s %s %s\n", head->op, getArgString(head->arg1), getArgString(head->arg2));
        }
        else if(head->type == INST_JUMP){
            printf("j %s\n", getArgString(head->arg1));
        }
        else if(head->type == INST_ALLOCATE_ARRAY_INT || head->type == INST_ALLOCATE_ARRAY_VAR){
            printf("%s = allocate(%s)\n", getArgString(head->arg1), getArgString(head->arg2));
        }
        else if(head->type == INST_ASSIGN_LW){
            printf("lw %s %s(%s)\n", getArgString(head->arg1), getArgString(head->arg2), getArgString(head->arg3));
        }
        else if(head->type == INST_ASSIGN_SW){
            printf("sw %s %s(%s)\n", getArgString(head->arg1), getArgString(head->arg2), getArgString(head->arg3));
        }
        else if(head->type == INST_WRITE_INT || head->type == INST_WRITE_STR){
            printf("write %s\n", getArgString(head->arg1));
        }
        else if(head->type == INST_READ){
            printf("read\n");
        }
        else if(head->type == INST_FUNCCALL){
            printf("functioncall %s\n", getArgString(head->arg1));
        }
        head = head->next;
//    }
}

Arg* getArrayCellValue(Expression* expr, int c){
    // to get the value in an array cell, we need to use an index and move the address manually
    Arg* index;
    if(expr->type!=CHAR){
        //if needed, multiply index by 4
        Expression* times4 = newExpression(INT, NULL, NULL, NULL, 4, NULL, NULL);
        Expression* multiplication = newExpression(INT, expr->left, times4, NULL, 0, "*", NULL);
        getBranchWeight(multiplication);
        index = cgen(multiplication, c);
        free(times4);
        free(multiplication);
    }
    else {
        index = cgen(expr->left, c);
    }
    // to get an item at index N, we say $t0 = arraypointer + index
    // to use the value, do lw $t1 0($t0)
    // to set a value, do sw $t2 0($t0)
    Arg* cell = newArg(ARG_REGISTER, c, "t");

    char buff[1000];
    snprintf(buff, 1000, "%s%i", expr->name, expr->envID);
    char* exprName = strdup(buff);

    Arg* name = newArg(ARG_VARIABLE, 0, exprName);

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
        free(times4);
        free(multiplication);
    }
    else {
        index = cgen(expr->left, c);
    }
    // to get an item at index N, we say $t0 = arraypointer + index
    // to use the value, do lw $t1 0($t0)
    // to set a value, do sw $t2 0($t0)
    Arg* cell = newArg(ARG_REGISTER, c, "t");

    char buff[1000];
    snprintf(buff, 1000, "%s%i", expr->name, expr->envID);
    char* exprName = strdup(buff);

    Arg* name = newArg(ARG_VARIABLE, 0, exprName);

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
            // whenever a variable is used, we must add the environment ID to it
            // this helps differentiate IDs that have the same name in different environments

            char buff[1000];
            snprintf(buff, 1000, "%s%i", expr->name, expr->envID);
            char* exprName = strdup(buff);


            Arg* varName = newArg(ARG_VARIABLE, 0, exprName);
            // if the value is unary negated, we need to convert to negative through multiplying by -1
            if(expr->isUnaryNegate){
                // add an expression (value * -1) and get the output $tX = value * -1
                expr->isUnaryNegate = false;
                Expression* negate = newExpression(INT, NULL, NULL, NULL, -1, NULL, NULL);
                Expression* multiplication = newExpression(INT, expr, negate, NULL, NULL, "*", NULL);
                Arg* negativeVal = cgen(multiplication, c);

                free(negate);
                free(multiplication);
                return negativeVal;
            }
            if(expr->isUnaryNot){
                // because we do not necessarily know the value, we must perform the unary NOT in the IR
                // this can be performed using seq $t0 value $zero. If it is 0, it is set to 1. otherwise, it is set to 0
                expr->isUnaryNot = false;
                Arg* arg = newArg(ARG_REGISTER, c, "t");
                Arg* zero = newArg(ARG_ZERO, 0, NULL);

                appendInstruction(program, newInstruction(INST_ASSIGN_OP, arg, varName, zero, "=="));
                return arg;
            }
            return varName;
        }
        else if(expr->type==FLOAT) {
            // if the type is float, generate the string and return it
            // if the type is a float, MIPS will need to load into a register and return it
            int value = (int)expr->fval;
            if(expr->isUnaryNegate){
                value = -value;
            }
            if(expr->isUnaryNot){
                value = !value;
            }
            return newArg(ARG_VALUE, value, NULL);
        }
        else if(expr->type == INT){
            // if the type is int, generate the string and return it
            // if there is a unary negate, change the ival to be negative
            if(expr->isUnaryNegate){
                expr->ival = -expr->ival;
            }
            if(expr->isUnaryNot){
                return newArg(ARG_VALUE, !expr->ival, NULL);
            } else {
                return newArg(ARG_VALUE, expr->ival, NULL);
            }
        }
        else if(expr->type == BOOL){
            //for unary NOT, change the value to !value if needed
            // because this is a boolean, we can perform the operation outside of the IR
            if(expr->isUnaryNot){
                return newArg(ARG_VALUE, !expr->ival, NULL);
            } else {
                return newArg(ARG_VALUE, expr->ival, NULL);
            }
        }
        else if(expr->type == STRING){
            return newArg(ARG_STRING, 0, expr->sval);
        }
        else if(expr->type == CHAR){
            return newArg(ARG_CHAR, 0, expr->sval);
        }
    }
    if(expr->isArrayCell){
        // if the expresion is an array cell (array name with a given index), it must be used like 0($t0)
        // if the value must be used, set the value on another register using lw $t1 0($t0)
        // if the value must be set, set it using sw $t1 0($t0)
        Arg* arrayCellValue = getArrayCellValue(expr, c);
        if(expr->isUnaryNegate){
            // add an instruction $tX = value * -1
            expr->isUnaryNegate = false;
            Arg* arg2 = newArg(ARG_REGISTER, c+1, "t");
            Arg* negate = newArg(ARG_VALUE, -1, NULL);

            appendInstruction(program, newInstruction(INST_ASSIGN_OP, arg2, arrayCellValue, negate, "*"));
            return arg2;
        }
        if(expr->isUnaryNot){
            // because we do not necessarily know the value, we must perform the unary NOT in the IR
            // this can be performed using seq $t0 value $zero. If it is 0, it is set to 1. otherwise, it is set to 0
            expr->isUnaryNot = false;
            Arg* arg = newArg(ARG_REGISTER, c, "t");
            Arg* zero = newArg(ARG_ZERO, 0, NULL);

            appendInstruction(program, newInstruction(INST_ASSIGN_OP, arg, arrayCellValue, zero, "=="));
            return arg;
        }
        return arrayCellValue;
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
        if(expr->isUnaryNegate){
            // add an instruction $tX = value * -1
            expr->isUnaryNegate = false;
            Arg* arg2 = newArg(ARG_REGISTER, c+1, "t");
            Arg* negate = newArg(ARG_VALUE, -1, NULL);

            appendInstruction(program, newInstruction(INST_ASSIGN_OP, arg2, arg, negate, "*"));
            return arg2;
        }
        return arg;
    }
}

void saveRegistersStack(){
    //todo this will only be done in the code generation.

    // save all "s" registers and $ra on the stack safely can safely be overwritten
    Arg* allRegs = newArg(ARG_VALUE, 36, NULL); // allocate enough space for all $s and $ra
    appendInstruction(program, newInstruction(INST_ALLOCATE_SP, allRegs, NULL, NULL, NULL));
    Arg* sp = newArg(ARG_SP, 0, "sp");


    for(int i=0; i<8; ++i) {
        Arg* sCurrent = newArg(ARG_REGISTER, i, "s");
        Arg* currentLoc = newArg(ARG_VALUE, i*4, NULL);
        appendInstruction(program, newInstruction(INST_ASSIGN_SW, sCurrent, currentLoc, sp, NULL));
    }
    //finally save $ra
    Arg* ra = newArg(ARG_RA, 0, "ra");
    Arg* currentLoc = newArg(ARG_VALUE, 32, NULL);
    appendInstruction(program, newInstruction(INST_ASSIGN_SW, ra, currentLoc, sp, NULL));
}

void loadRegistersStack(){
    //todo this will only be done in the code generation.

    // reload all of the saved registers and free the space on the stack
    Arg* sp = newArg(ARG_SP, 0, "sp");
    for(int i=0; i<8; ++i) {
        Arg* sCurrent = newArg(ARG_REGISTER, i, "s");
        Arg* currentLoc = newArg(ARG_VALUE, i*4, NULL);
        appendInstruction(program, newInstruction(INST_ASSIGN_LW, sCurrent, currentLoc, sp, NULL));
    }
    //finally save $ra
    Arg* ra = newArg(ARG_RA, 0, "ra");
    Arg* currentLoc = newArg(ARG_VALUE, 32, NULL);
    appendInstruction(program, newInstruction(INST_ASSIGN_LW, ra, currentLoc, sp, NULL));

    // free the space that was allocated on the stack
    Arg* allRegs = newArg(ARG_VALUE, 36, NULL); // free the space for all $s and $ra
    appendInstruction(program, newInstruction(INST_FREE_SP, allRegs, NULL, NULL, NULL));
}



char* cgenStatement(Statement* stmt){
    while(stmt!=NULL) {
        //printf("========================\n");
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
            if(stmt->expr->isFunctionCall){
                // jal functionName
                // generate the start label and then jump there
                char buff[1000];
                snprintf(buff, 1000, "_%s", stmt->expr->name);
                char* str = strdup(buff);
                Arg* funcLabel = newArg(ARG_LABEL, 0, str);


                // todo save all registers in stack in MIPS
                //saveRegistersStack();
                // push parameters into the stack?
                Arg* size = newArg(ARG_VALUE, stmt->expr->args->size, NULL);
                appendInstruction(program, newInstruction(INST_ALLOCATE_SP, size, NULL, NULL, NULL));

                Argument* temp = stmt->expr->args->head;
                while(temp!=NULL){

                    temp = temp->next;
                }
                appendInstruction(program, newInstruction(INST_FUNCCALL, funcLabel, NULL, NULL, NULL));

                //todo restore all "s" registers and $ra from the stack in MIPS
                //loadRegistersStack();
            }
            getBranchWeight(stmt->expr);
            cgen(stmt->expr, 0);
        }
        else if(stmt->type == STMT_BLOCK){
            cgenStatement(stmt->codeBody);
        }
        else if(stmt->type == STMT_DECL){
            if(stmt->decl->type==FUNCTION){
                currentFunctionName = stmt->decl->name;
                // generate the start and end labels
                char buff[1000];
                snprintf(buff, 1000, "_%s", stmt->decl->name);
                char* str = strdup(buff);
                Arg* startLabel = newArg(ARG_LABEL, 0, str);

                snprintf(buff, 1000, "_%s_END", stmt->decl->name);
                str = strdup(buff);
                Arg* endLabel = newArg(ARG_LABEL, 0, str);

                // the skip label is used so that function declarations do not interfere with other code
                // this also ensures that the function is not run unless called
                snprintf(buff, 1000, "_%s_SKIP", stmt->decl->name);
                str = strdup(buff);
                Arg* skipLabel = newArg(ARG_LABEL, 0, str);

                appendInstruction(program, newInstruction(INST_JUMP, skipLabel, NULL, NULL, NULL));

                //insert the start label and start function instructions
                appendInstruction(program, newInstruction(INST_DOT_ENT, startLabel, NULL, NULL, NULL));
                appendInstruction(program, newInstruction(INST_LABEL, startLabel, NULL, NULL, NULL));
                appendInstruction(program, newInstruction(INST_START_FUNC, NULL, NULL, NULL, NULL));
                // save registers + $ra

                cgenStatement(stmt->decl->codeBlock);

                //end the function
                appendInstruction(program, newInstruction(INST_LABEL, endLabel, NULL, NULL, NULL));
                appendInstruction(program, newInstruction(INST_END_FUNC, NULL, NULL, NULL, NULL));
                // reload saved registers +$ra
                // jr ra here
                appendInstruction(program, newInstruction(INST_JR_RA, NULL, NULL, NULL, NULL));

                appendInstruction(program, newInstruction(INST_DOT_END, startLabel, NULL, NULL, NULL));

                appendInstruction(program, newInstruction(INST_LABEL, skipLabel, NULL, NULL, NULL));



            }
            if(!stmt->decl->isArray && stmt->decl->value!=NULL){
                //the declaration is of the form "type x = expr"
                //this becomes just a normal assign in the IR
                Arg* value;
                value = cgen(stmt->decl->value, 0);

                char buff[1000];
                snprintf(buff, 1000, "%s%i", stmt->decl->name, stmt->decl->envID);
                char* exprName = strdup(buff);

                Arg* name = newArg(ARG_VARIABLE, 0, exprName);

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
                    char buff[1000];
                    snprintf(buff, 1000, "%s%i", stmt->decl->name, stmt->decl->envID);
                    char* exprName = strdup(buff);
                    Arg* name = newArg(ARG_VARIABLE, 0, exprName);
                    appendInstruction(program,
                                      newInstruction(INST_ALLOCATE_ARRAY_INT, name, sizearg, NULL, NULL));
                } else {
                    Arg* name = newArg(ARG_VARIABLE, 0, stmt->decl->name);

                    char buff[1000];
                    snprintf(buff, 1000, "%s%i", stmt->decl->value->name, stmt->decl->value->envID);
                    char* exprName = strdup(buff);

                    Arg* varname = newArg(ARG_VARIABLE, 0, stmt->decl->value->name);
                    //the array is allocated using a variable
                    appendInstruction(program,
                                      newInstruction(INST_ALLOCATE_ARRAY_VAR, name, varname, NULL, NULL));

                }
            }
        }
        else if(stmt->type == STMT_RETRN){
            // assign $v0 = value
            Arg* v0 = newArg(ARG_REGISTER, 0, "v");
            Arg* value = cgen(stmt->expr, 0);
            appendInstruction(program, newInstruction(INST_ASSIGN, v0, value, NULL, NULL));
            // jump to the end of the function
            // now generate the label for the end of the function
            char buff[1000];
            snprintf(buff, 1000, "_%s_END", currentFunctionName);
            char* str = strdup(buff);
            Arg* endLabel = newArg(ARG_LABEL, 0, str);
            appendInstruction(program, newInstruction(INST_JUMP, endLabel, NULL, NULL, NULL));

        }
        else if(stmt->type == STMT_WRITE){
            Arg* value = cgen(stmt->expr, 0);
            if(stmt->expr->type==INT) {
                appendInstruction(program, newInstruction(INST_WRITE_INT, value, NULL, NULL, NULL));
            } else{
                appendInstruction(program, newInstruction(INST_WRITE_STR, value, NULL, NULL, NULL));
            }
        }
        else if(stmt->type == STMT_WRITELN){
            // todo a system string will always exist in the data segment with this name _NEWLINE0
            Arg* value = newArg(ARG_LABEL, 0, "_NEWLINE");
            appendInstruction(program, newInstruction(INST_WRITE_STR, value, NULL, NULL, NULL));
        }
        else if(stmt->type == STMT_READ){
            appendInstruction(program, newInstruction(INST_READ, NULL, NULL, NULL, NULL));

        }
        stmt = stmt->next;
    }
}

bool areArgsEqual(Arg* arg1, Arg* arg2){
    if(arg1->type!=arg2->type){
        return false;
    }
    else {
        return (strcmp(arg1->name, arg2->name)==0) && (arg1->value == arg2->value);
    }
}

int calculateExpression(Arg* arg1, Arg* arg2, char* op){
    if(strcmp(op, "+")==0){
        return arg1->value + arg2->value;
    }
    else if(strcmp(op, "-")==0){
        return arg1->value - arg2->value;
    }
    else if(strcmp(op, "*")==0){
        return arg1->value * arg2->value;
    }
    else if(strcmp(op, "/")==0){
        return arg1->value / arg2->value;
    }
    else if(strcmp(op, "==")==0){
        return arg1->value == arg2->value;
    }
    else if(strcmp(op, "!=")==0){
        return arg1->value != arg2->value;
    }
    else if(strcmp(op, ">")==0){
        return arg1->value > arg2->value;
    }
    else if(strcmp(op, "<")==0){
        return arg1->value < arg2->value;
    }
    else if(strcmp(op, ">=")==0){
        return arg1->value >= arg2->value;
    }
    else if(strcmp(op, "<=")==0){
        return arg1->value <= arg2->value;
    }
}

// the Program data structure can be repurposed to be used for basic blocks
// basic blocks are sections of code that are always run together
// whenever a jump occurs, a new basic block is started
bool isBlockEnd(Instruction* inst){
    int endBlockTypes[8] = {INST_LABEL, INST_START_FUNC, INST_END_FUNC, INST_DOT_ENT,
                           INST_JR_RA, INST_FUNCCALL, INST_JUMP, INST_COND_JUMP};
    for(int i=0; i<8; ++i){
        if(inst->type==endBlockTypes[i]){
            return true;
        }
    }
    return false;
}

bool isAssign(Instruction* inst){
    int endBlockTypes[5] = {INST_ASSIGN, INST_ASSIGN_OP, INST_ASSIGN_LW,
                            INST_ALLOCATE_ARRAY_VAR, INST_ALLOCATE_ARRAY_INT}; // remove sw?
    for(int i=0; i<5; ++i){
        if(inst->type==endBlockTypes[i]){
            return true;
        }
    }
    return false;
}

Program* getBasicBlock(Instruction* head){
    Program* basicBlock = malloc(sizeof(Program));
    if(basicBlock==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    basicBlock->head = basicBlock->tail = head;
    // getting the basic block
    Instruction* current = head->next;
    int size = 1;
    while(current!=NULL && !isBlockEnd(current)){
        size++;
        appendInstruction(basicBlock, current);
        current = current->next;
    }
//    printf("blocksize: %i\n", size);
    return basicBlock;
}
bool simpleExpressionEvaluation(Program* block){
    bool ranOptimization = false;
    // OPTIMIZATION #1:
    //  simple expression calculation
    //  when there is a expression of the form "1 + 2", it is more effective to calculate it immediately
    //  this can save calculations when they are run many time in MIPS
    Instruction* current = block->head;
    // loop through all instructions in the block
    while(current!=NULL) {
        if (current->type == INST_ASSIGN_OP) {
            if (current->arg2->type == ARG_VALUE && current->arg3->type == ARG_VALUE) {
                // run calculation
                int value = calculateExpression(current->arg2, current->arg3, current->op);
                current->type = INST_ASSIGN;
                current->arg2->value = value;
                free(current->arg3);
                ranOptimization = true;
            }

        }
        current = current->next;
    }
    return ranOptimization;
}
bool isValueUsed(Arg* value, Instruction* head, Instruction* tail){
    // traverse through the given section of code and find if the value is ever used
    // if the value is assigned again, the check can stop there
    Instruction* current = head;
//    printf("is val %s used?\n", value->name);
    while(current!=tail->next && current!=NULL){
        if(current->arg2!=NULL && (areArgsEqual(current->arg2, value))) {
            return true;
        }
        if(current->arg3!=NULL && areArgsEqual(current->arg3, value)) {
            return true;
        }

        if(isAssign(current)){
            if(areArgsEqual(current->arg1, value)){
                // the variable is being set to a different value without using the original one, break
                return false;
            }
        }
        else {
            if(current->arg1!=NULL && areArgsEqual(current->arg1, value)) {
                return true;
            }
        }
        current = current->next;
    }
    return true;
}



bool deadCodeRemoval(Program* block, Program* program){
    bool ranOptimization = false;
    // OPTIMIZATION #2:
    //  run a liveness analysis on each variable in the block
    //  if a variable is assigned to a value but that value is never used, there is no need to assign it
    Instruction* current = block->head;
    Instruction* next;

    while(current!=NULL && current!=block->tail){
        next = current->next;
        if(isAssign(current)){
            // if the instruction is an assign, check all of the instructions in the block and see if the value is used
            // check all instructions after the current one
            if(!isValueUsed(current->arg1, current->next, block->tail)){
                // if this is the head or tail of the block, move the head/tail
                if(block->head==block->tail){
                    block->head = block->tail = NULL;
                }
                if(current == block->head){
                    block->head = block->head->next;
                }
                if(current == block->tail){
                    block->tail = block->tail->prev;
                }
                // remove the instruction
                removeInstruction(current, program);
                ranOptimization = true;
            }
        }
        current = next;
    }
    return ranOptimization;
}

bool propagateCostant(Arg* var, Arg* value, Instruction* head, Instruction* tail){
    bool ranOptimization = false;
    // take the given variable and set all future uses of it to the value, if possible
    Instruction* current = head;
    while(current!=NULL && current!=tail->next){
        if(isAssign(current) && areArgsEqual(current->arg1, value)){
            // the variable is reassigned to a new value, stop constant propagation
            break;
        }
        else if(current->type == INST_ASSIGN_OP || current->type == INST_ASSIGN){
            // if the instruction is an assign or assign op, we can propagate the constant if possible
            if(current->arg2 != NULL && areArgsEqual(current->arg2, var)){
                Arg* newArgument = newArg(value->type, value->value, value->name);
                current->arg2 = newArgument;
                ranOptimization = true;
//                printf("ran opt:\n");
//                emitPrintStatement(current);
//                printf("\n");
            }
            if(current->arg3 != NULL && areArgsEqual(current->arg3, var)){
                Arg* newArgument = newArg(value->type, value->value, value->name);
                current->arg3 = newArgument;
                ranOptimization = true;
//                printf("ran opt\n");
//                emitPrintStatement(current);
//                printf("\n");
            }
        }
        else if(current->type == INST_WRITE_INT || current->type == INST_WRITE_STR){
            if(current->arg1 != NULL && areArgsEqual(current->arg1, var)){
                Arg* newArgument = newArg(value->type, value->value, value->name);
                current->arg1 = newArgument;
                ranOptimization = true;
//                printf("ran opt\n");
//                emitPrintStatement(current);
//                printf("\n");
            }
        }
        current = current->next;
    }
    return ranOptimization;
}

bool constantPropogation(Program* block){
    bool ranOptimization = false;
    // OPTIMIZATION #3:
    //  If a variable/register is set to a constant value, set all future uses to that value
    //  This saves time so that the variable does not have to be read many times
    //  this also opens up possibilities for other optimizations such as simplifying entire math expressions
    Instruction* current = block->head;
    while(current!=NULL && current!=block->tail){
        if(current->type == INST_ASSIGN){
            // if the register/variable is assigned to a constant, edit all future uses of it (as possible)
            ranOptimization = propagateCostant(current->arg1, current->arg2, current->next, block->tail);
        }
        current = current->next;
    }
    return ranOptimization;
}

void optimizeIR(Program* program){
    Instruction* current = program->head;
    // loop through all basic blocks in the program and run optimizations
    while(current!=NULL) {
        Program* basicBlock = getBasicBlock(current);
        current = basicBlock->tail->next;

        // Optimization 1
        simpleExpressionEvaluation(basicBlock);

        // Optimization 2
        deadCodeRemoval(basicBlock, program);

        // Optimization 3
        constantPropogation(basicBlock);
    }

}