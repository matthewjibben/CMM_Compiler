//
// Created by Matthew Jibben on 4/18/2019.
//

#ifndef SYMBOLTABLE_IR_H
#define SYMBOLTABLE_IR_H

#include "ast.h"

typedef enum instrType{
    INST_LABEL,
    INST_ASSIGN,
    INST_ASSIGN_OP,
    INST_ALLOCATE_ARRAY_VAR,
    INST_ALLOCATE_ARRAY_INT,
    INST_ASSIGN_UNARY,
    INST_JUMP,
    INST_COND_JUMP,
    INST_RELOP_JUMP,
    PROCEDURE_CALL,     //is this necessary? in the book, it is really many instructions ... possibly JAL
    INDEX_ASSIGN,       // x[i] = y
    ASSIGN_INDEX        // x = y[i]
} instrType;

typedef struct Instruction {
    instrType type;
    char* arg1;
    char* arg2;
    char* arg3;
    char* op;
    struct Instruction* next;
} Instruction;

typedef struct Program {
    Instruction* head;
    Instruction* tail;
} Program;




Instruction* newInstruction(instrType type, char* arg1, char* arg2, char* arg3, char* op);
void appendInstruction(Program* prog, Instruction* instr);

int getBranchWeight(Expression* expr);
char* cgen(Expression* expr, int c);
char* cgenStatement(Statement* stmt);


#endif //SYMBOLTABLE_IR_H
