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
    ARG_STRING,
    ARG_CHAR,
    ARG_SP,
    ARG_RA,
    ARG_ZERO
} argType;

typedef struct Arg {
    argType type;

    // for a register, the name is a char EX: "t" or "s". the value is the number. This builds: "t0" for example
    // for a variable, only the name is used
    // for a value, only the int value is used
    // for a label, both name and value are used
    int value;
    char* name;

    int dataType;
} Arg;

typedef enum instrType{
    INST_LABEL,
    INST_ASSIGN,
    INST_ASSIGN_OP,
    INST_ASSIGN_LW,
    INST_ASSIGN_SW,
    INST_ALLOCATE_ARRAY_VAR,
    INST_ALLOCATE_ARRAY_INT,
    INST_WRITE_STR,
    INST_WRITE_INT,
    INST_WRITE_CHR,
    INST_WRITELN,
    INST_READ,
    INST_START_FUNC,
    INST_END_FUNC,
    INST_DOT_ENT,
    INST_DOT_END,
    INST_JR_RA,
    INST_FUNCCALL, // for a function call we will need to: store each "s" in $sp, and store $ra. Store each parameter in $sp. jal function. after, we must delete the parameters, load all the "s" variables and $ra back
    INST_ALLOCATE_SP,
    INST_FREE_SP,
    INST_ASSIGN_UNARY,
    INST_JUMP,
    INST_COND_JUMP,
    INST_RELOP_JUMP,
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
    struct Instruction* prev;

    bool gettingCellPointer;
} Instruction;






typedef struct Program {
    Instruction* head;
    Instruction* tail;
} Program;




Instruction* newInstruction(instrType type, Arg* arg1, Arg* arg2, Arg* arg3, char* op);
void appendInstruction(Program* prog, Instruction* instr);
void removeInstruction(Instruction* instr, Program* program);
bool areArgsEqual(Arg* arg1, Arg* arg2);
char* getArgString(Arg* arg);
Arg* newArg(argType type, int value, char* name);

int getBranchWeight(Expression* expr);
Arg* cgen(Expression* expr, int c);
char* cgenStatement(Statement* stmt);
void emitPrintStatement(Instruction* instruction);

#endif //SYMBOLTABLE_IR_H
