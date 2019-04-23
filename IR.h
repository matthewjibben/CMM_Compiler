//
// Created by Matthew Jibben on 4/18/2019.
//

#ifndef SYMBOLTABLE_IR_H
#define SYMBOLTABLE_IR_H

#include "ast.h"

typedef enum argType{
    ARG_REGISTER,
    ARG_VARIABLE,
    ARG_VALUE,
    ARG_LABEL,
    ARG_STRING
} argType;

typedef struct Arg {
    argType type;

    // for a register, the name is a char EX: "t" or "s". the value is the number. This builds: "t0" for example
    // for a variable, only the name is used
    // for a value, only the int value is used
    // for a label, both name and value are used
    int value;
    char* name;
} Arg;

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
    Arg* arg1;
    Arg* arg2;
    Arg* arg3;
    char* op;
    struct Instruction* next;
} Instruction;






typedef struct Program {
    Instruction* head;
    Instruction* tail;
} Program;




Instruction* newInstruction(instrType type, Arg* arg1, Arg* arg2, Arg* arg3, char* op);
void appendInstruction(Program* prog, Instruction* instr);

int getBranchWeight(Expression* expr);
Arg* cgen(Expression* expr, int c);
char* cgenStatement(Statement* stmt);


#endif //SYMBOLTABLE_IR_H
