//
// Created by Matthew Jibben on 2/5/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include "cmm.tab.h"

extern int yyparse();
extern FILE* yyin;

int main(int argc, char **argv)
{
    printf("=========================parser.c===============================================\n");
    //open the input sample cmm file
    if((yyin = fopen("./sample.cmm","r"))==NULL){
        printf("Failed to read file");
        exit(1);
    }
    else {
        printf("file correctly opened\n");
    }

    //int c;
    //while((c=getc(yyin))!=EOF){
    //    putchar(c);
    //}


    printf("\n\nyyparse: %i\n", yyparse());
    fclose(yyin);
    printf("exit\n\n");
    return 0;
}
