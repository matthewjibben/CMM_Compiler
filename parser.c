//
// Created by Matthew Jibben on 2/5/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include "cmm.tab.h"
#include "ast.h"
#include "symboltable.h"

extern int yyparse();
extern FILE* yyin;
extern FILE* output;
extern Env* env;
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


//    output = fopen("./output.asm", "w");
//    fprintf(output, ".data\nnewline: .asciiz \"\\n\"\n.text\n.globl main\nmain:\n");
    // create initial global environment
    env = newEnvironment(NULL);
    //Declaration* decl1 = newDeclaration("testName", false, STRING, NULL, NULL, NULL, NULL, NULL);
    //Env* env = newEnvironment(NULL);
    //insertEntry(env, decl1);

    // begin
    printf("\n\nProcess finished with exit code: %i\n\n\n", yyparse());

    //printEnv(env);
//    fprintf(output, "exit:\n");
//    fprintf(output, "li $v0 4\t \t# load the print string syscall\n");
//    fprintf(output, "la $a0 newline \t# load the string \n");
//    fprintf(output, "syscall\n");
//    fprintf(output, "li $v0 10\n");
//    fprintf(output, "syscall\n");

    //freeDeclaration(decl1);
    freeEnv(env);

    fclose(yyin);
//    fclose(output);
    return 0;
}
