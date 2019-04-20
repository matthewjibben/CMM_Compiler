//
// Created by Matthew Jibben on 4/18/2019.
//

#ifndef SYMBOLTABLE_IR_H
#define SYMBOLTABLE_IR_H

typedef enum instrType{
    INST_LABEL,
    INST_ASSIGN,
    INST_ASSIGN_OP,
    INST_ASSIGN_UNARY,
    INST_JUMP,
    INST_COND_JUMP,
    INST_RELOP_JUMP,
    PROCEDURE_CALL,     //is this necessary? in the book, it is really many instructions
    INDEX_ASSIGN,       // x[i] = y
    ASSIGN_INDEX        // x = y[i]
} instrType;

typedef struct Instruction {
    instrType type;
    char* arg1;
    char* arg2;
    char* arg3;
    char* op;
} Instruction;


#endif //SYMBOLTABLE_IR_H
