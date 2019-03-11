//
// Created by Matthew Jibben on 2/5/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include "cmm.tab.h"

extern int yyparse();
extern FILE* yyin;
//extern FILE* output;
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
//    fprintf(output, ".data\n.text\n.globl main\nmain:\n");

    printf("\n\nProcess finished with exit code: %i\n\n\n", yyparse());

    fclose(yyin);
    return 0;
}
