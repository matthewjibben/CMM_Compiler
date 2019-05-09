//
// Created by Matthew Jibben on 5/6/2019.
//

#ifndef CODEGEN_H
#define CODEGEN_H


#include <stdio.h>
#include "IR.h"
#include "ast.h"
#include "symboltable.h"

void printAssign(Instruction* instruction, FILE* output);
void printInstruction(Instruction* instruction, FILE* output);
void codeGen(Program* program, FILE* output);


#endif //CODEGEN_H
