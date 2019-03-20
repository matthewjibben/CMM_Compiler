//
// Created by Matthew Jibben on 2/5/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include "cmm.tab.h"

extern int yyparse();
extern FILE* yyin;
extern FILE* output;
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

    printf("\n\nProcess finished with exit code: %i\n\n\n", yyparse());

//    fprintf(output, "exit:\n");
//    fprintf(output, "li $v0 4\t \t# load the print string syscall\n");
//    fprintf(output, "la $a0 newline \t# load the string \n");
//    fprintf(output, "syscall\n");
//    fprintf(output, "li $v0 10\n");
//    fprintf(output, "syscall\n");

    fclose(yyin);
//    fclose(output);
    return 0;
}
