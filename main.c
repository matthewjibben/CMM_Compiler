#include <stdio.h>
#include <stdlib.h>
#include "symboltable.h"



int main() {
    struct Env x;
    printf("0\n");
    initEnv(&x, NULL);
    printf("1\n");
    insertEntry(&x, "id1", "type1", "vpointer1");
    printf("2\n");
    insertEntry(&x, "id2", "type2", "vpointer2");
    printf("3\n");
    insertEntry(&x, "id3", "type3", "vpointer2");
    printf("4\n");
    insertEntry(&x, "id4", "type4", "vpointer2");
    printf("5\n");
    insertEntry(&x, "id5", "type5", "vpointer2");
    printf("6\n");
    printf("symbols:\n");
    printEnv(&x);

    printf("main delete\n\n");
    deleteItem(&x, "id1");
    deleteItem(&x, "id2");
    deleteItem(&x, "id3");
    deleteItem(&x, "id4");
    deleteItem(&x, "id5");
    printEnv(&x);

    return 0;
}