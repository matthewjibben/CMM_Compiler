//
// Created by Matthew Jibben on 5/6/2019.
//

#include "codeGen.h"
extern Env* mipsEnv;


Arg* getVariable(Arg* var, FILE* output){
    //prints instructions to file that get a variable from the .data section and places it in $a0
    fprintf(output, "la $a0 %s\nlw $a0 0($a0)\n", getArgString(var));
    return newArg(ARG_REGISTER, 0, "a");    // return $a0
}

Arg* getVarAddress(Arg* var, FILE* output){
    //return the address of a variable given it is already defined
    fprintf(output, "la $a0 %s\n", getArgString(var));
    return newArg(ARG_REGISTER, 0, "a");    // return $a0
}

void setVarValue(Arg* var, Arg* value, FILE* output){
    // given a variable that is already defined, get the address and change the value
    if(var->dataType==INT || var->dataType==BOOL) {
        char* moveType;
        if(value->type==ARG_VALUE){
            moveType = "li";
        }
        else if(value->type==ARG_REGISTER){
            moveType = "move";
        }
        //first set the value, to make sure it is saved
        fprintf(output, "%s $a1 %s\n", moveType, getArgString(value));
        // load the address and set
        fprintf(output, "la $a0 %s\n", getArgString(var));
        fprintf(output, "sw $a1 0($a0)\n");

    }
}

void defineVar(Instruction* instruction, FILE* output){
    // use this function when there is a instruction of the form "VARIABLE = anything"
    // if this is the first time using that variable, define it in the .text segment
    char* arg = getArgString(instruction->arg1);
    if(lookup(mipsEnv, arg)==NULL) {
        // if the variable does not exist in the environment, it should be added and declared in MIPS
        insertEntry(mipsEnv, newDeclaration(arg, false, instruction->arg1->dataType, NULL, NULL, NULL, NULL, NULL));
        if(instruction->arg1->dataType==INT || instruction->arg1->dataType==BOOL) {
            fprintf(output, ".data\n %s: .word %s\n.text\n", arg, getArgString(instruction->arg2));
        }
        else if(instruction->arg1->dataType==CHAR){
            fprintf(output, ".data\n %s: .byte %s\n.text\n", arg, getArgString(instruction->arg2));
        }
        else if(instruction->arg1->dataType==STRING){
            fprintf(output, ".data\n %s: .asciiz %s\n.text\n", arg, getArgString(instruction->arg2));
        }
    }
    else {
        // if this is not the first time using a variable, get the address and set it
        char* moveType;
        if(instruction->arg2->type==ARG_VALUE || instruction->arg2->type==ARG_CHAR){
            moveType = "li";
        }
        else if(instruction->arg2->type==ARG_REGISTER){
            moveType = "move";
        }
        //first set the value, to make sure it is saved
        fprintf(output, "%s $a1 %s\n", moveType, getArgString(instruction->arg2));
        // load the address and set
        fprintf(output, "la $a0 %s\n", arg);
        fprintf(output, "sw $a1 0($a0)\n");
    }
}

void printAssign(Instruction* instruction, FILE* output){
    if(instruction->type!=INST_ASSIGN){exit(1);}
    // must check the kind of assign
    // if a register is set to another register, use 'move'
    if(instruction->arg1->type == ARG_REGISTER && instruction->arg2->type == ARG_REGISTER) {
        fprintf(output, "move %s %s\n", getArgString(instruction->arg1), getArgString(instruction->arg2));
    }
    else if(instruction->arg1->type == ARG_REGISTER &&
                (instruction->arg2->type == ARG_VALUE || instruction->arg2->type == ARG_CHAR)) {
        fprintf(output, "li %s %s\n", getArgString(instruction->arg1), getArgString(instruction->arg2));
    }
    else if(instruction->arg1->type == ARG_REGISTER && instruction->arg2->type == ARG_VALUE) {
        fprintf(output, "li %s %s\n", getArgString(instruction->arg1), getArgString(instruction->arg2));
    }
    else if(instruction->arg1->type == ARG_REGISTER && instruction->arg2->type == ARG_VARIABLE) {
        Arg* getvar = getVariable(instruction->arg2, output);
        fprintf(output, "move %s %s\n", getArgString(instruction->arg1), getArgString(getvar));
    }
    else if(instruction->arg1->type == ARG_VARIABLE){
        defineVar(instruction, output);
    }
    else{
        printf("other?\n");
    }
}

void printInstruction(Instruction* instruction, FILE* output){
    if(instruction->type == INST_LABEL){
        fprintf(output, "%s%i:\n", instruction->arg1->name, instruction->arg1->value);
    }
    if(instruction->type == INST_ASSIGN){
        printAssign(instruction, output);
    }
}

void codeGen(Program* program, FILE* output) {
    // create an environment for the MIPS code. This will help in defining variables
    // mipsEnv is created in parser.c
    // traverse the linked list of IR instructions, converting it to MIPS instructions
    Instruction* current = program->head;
    while(current!=NULL){
        //print each line
        printInstruction(current, output);
        current = current->next;
    }
}