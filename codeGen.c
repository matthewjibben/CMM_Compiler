//
// Created by Matthew Jibben on 5/6/2019.
//

#include "IR.h"
#include "ast.h"
#include "symboltable.h"

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
}

void printInstruction(Instruction* instruction, FILE* output){
    if(instruction->type == INST_LABEL){
        fprintf(output, "%s:\n", instruction->arg1->name);
    }
    if(instruction->type == INST_ASSIGN){
        printAssign(instruction, output);
    }
}

void codeGen(Program* program, FILE* output){
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