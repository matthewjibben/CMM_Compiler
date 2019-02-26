//
// Created by Matthew Jibben on 1/29/2019.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdio.h>
#include <stdlib.h>

// =======================   Symbol    =======================
typedef struct Symbol {
    // the 2d array includes 3 elements in each row:
    // 0: identifier
    // 1: type
    // 2: value pointer. This will be able to be updated in the future as needed
    char* id;
    char* type;
    char* value;
    //I use a doubly linked list of symbols so that deletion is easier
    struct Symbol* next;
    struct Symbol* prev;
} Symbol;

void deleteSym(struct Symbol* sym);
// =======================   Symbol    =======================

// ======================= Environment =======================
typedef struct Env{
    int size;
    struct Symbol* head;    // head of the linked list of symbols
    struct Symbol* tail;
    struct Env* prev;
} Env ;

void initEnv(struct Env* env, struct Env* previous);
int insertEntry(struct Env* env, char* id, char* type, char* value);
struct Symbol* lookup(struct Env* env, char* id);
void deleteItem(struct Env* env, char* id);
void printEnv(struct Env* env);
void updateVal(struct Env* env, char* id, char* newval);
void updateType(struct Env* env, char* id, char* newtype);
void freeEnv(struct Env* env);
// ======================= Environment =======================



#endif //SYMBOLTABLE_H
