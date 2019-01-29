//
// Created by Matthew Jibben on 1/29/2019.
//

#ifndef SYMBOLTABLE_SYMBOLTABLE_H
#define SYMBOLTABLE_SYMBOLTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

// =======================   Symbol    =======================
typedef struct Symbol {
    /* the 2d array includes 3 elements in each row:
     * 0: identifier
     * 1: type
     * 2: value pointer. This will be able to be updated in the future as needed
     */
    char* id;
    char* type;
    char* vpointer;
    //I use a doubly linked list of symbols so that deletion is easier
    struct Symbol* next;
    struct Symbol* prev;
};
// =======================   Symbol    =======================

// ======================= Environment =======================
typedef struct Env {
    int size;
    struct Symbol* table;
    struct Env* prev;

    // MAKE AN ARRAY OF POINTERS TO SYMBOLS
    // THAT WAY YOU ONLY HAVE TO ADD ANOTHER 4 BYTES IF YOU NEED A NEW POINTER
};

void initEnv(struct Env* env, struct Env* previous){
    //set the size to 0, it will be changed once we make insertions
    env->size = 0;
    //The table currently has no elements, the array will be increased as needed
    env->table = malloc(0);
    env->prev = previous;
}

int insertEntry(struct Env* env, char* id, char* type, char* vpointer){
    // allocate the size of the original memory plus enough space for one more pointer
    int symSize = sizeof(struct Symbol);
    void* newMem = realloc(env->table, (size_t)(symSize*env->size + symSize));
    if(!newMem){
        printf("allocating memory somehow failed");
        abort();
    }
    env->table = newMem;

    struct Symbol temp = {id, type, vpointer};

    //printf("size of temp: %i\n", sizeof(struct Symbol));
    env->table[env->size] = temp;
    env->size++;
    // we will return the index in the table where it is inserted
    return env->size-1;
}

struct Symbol* lookup(struct Env* env, char* id){
    // lookup through each environment until we find the id
    for(struct Env* tempenv = env; tempenv != NULL; tempenv = tempenv->prev){
        // search through the table
        for(int i=0; i<tempenv->size; ++i){
            if(tempenv->table[i].id == id){
                return &tempenv->table[i];
            }
        }
    }
    // if the value is never reached
    return NULL;
}

void deleteItem(struct Env* env, char* id){
    // perform the same operations as a lookup, but then delete the entry
    // is malloc even a good idea? linked list of symbols?
}

// ======================= Environment =======================



#endif //SYMBOLTABLE_SYMBOLTABLE_H
