//
// Created by Matthew Jibben on 2/5/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include "cmm.tab.h"
#include "ast.h"
#include "symboltable.h"
#include "IR.h"

extern int yyparse();
extern FILE* yyin;
extern FILE* output;
extern FILE* outputTAC;
extern Env* env;
extern Env* mipsEnv;
extern Program* program;
int main(int argc, char **argv)
{
    printf("=========================parser.c=========================\n");
    //open the input sample cmm file
    if((yyin = fopen("./sample.cmm","r"))==NULL){
        printf("Failed to read file");
        exit(1);
    }
    else {
        printf("file correctly opened\n");
    }
    program = malloc(sizeof(program));

    outputTAC = fopen("./output.tac", "w");
    fprintf(outputTAC, "===TAC FILE===\n");

    output = fopen("code.asm", "w");
    fprintf(output, ".data\n_newline: .asciiz \"\\n\"\n.text\n.globl main\nmain:\n");

    // create initial global environment
    mipsEnv = newEnvironment(NULL);
    env = newEnvironment(NULL);
    env->type = 0; //global type
    //Declaration* decl1 = newDeclaration("testName", false, STRING, NULL, NULL, NULL, NULL, NULL);
    //Env* env = newEnvironment(NULL);
    //insertEntry(env, decl1);

    // begin
    printf("\n\nProcess finished with exit code: %i\n\n\n", yyparse());

    //add exit syscall for MIPS
    fprintf(output, "\n__exit: \nli $v0 10\nsyscall");


    //printEnv(env);
//    fprintf(output, "exit:\n");
//    fprintf(output, "li $v0 4\t \t# load the print string syscall\n");
//    fprintf(output, "la $a0 newline \t# load the string \n");
//    fprintf(output, "syscall\n");
//    fprintf(output, "li $v0 10\n");
//    fprintf(output, "syscall\n");

    //freeDeclaration(decl1);
    free(program);  //todo free all instructions too
    freeEnv(env);

    fclose(yyin);
//    fclose(output);
    return 0;
}
