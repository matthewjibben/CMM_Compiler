//
// Created by Matthew Jibben on 1/29/2019.
//

#ifndef SYMBOLTABLE_SYMBOLTABLE_H
#define SYMBOLTABLE_SYMBOLTABLE_H

#include <stdio.h>
#include <stdlib.h>

// =======================   Symbol    =======================
struct Symbol {
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
};

void deleteSym(struct Symbol* sym){
    free(sym->id);
    free(sym->type);
    free(sym->value);
    free(sym);
}

// =======================   Symbol    =======================

// ======================= Environment =======================
struct Env {
    int size;
    struct Symbol* head;    // head of the linked list of symbols
    struct Symbol* tail;
    struct Env* prev;
};

void initEnv(struct Env* env, struct Env* previous){
    //set the size to 0, it will be changed once we make insertions
    env->size = 0;
    //The table currently has no elements, the array will be increased as needed
    env->prev = previous;
    env->head = env->tail = NULL;
}

int insertEntry(struct Env* env, char* id, char* type, char* value){
    struct Symbol* temp = malloc(sizeof(struct Symbol));
    temp->id = id;
    temp->type = type;
    temp->value = value;
    temp->next = NULL;
    temp->prev = NULL;

    //if this is the first symbol, set it to the head
    if(env->head==NULL){
        env->head = env->tail = temp;
    }
    else {
        //otherwise, use the tail to add the new symbol
        //first, link the new symbols together, then set the new symbol as the tail
        temp->prev = env->tail;
        env->tail->next = temp;
        env->tail = temp;
    }
    env->size++;
    // we will return the index in the table where it is inserted
    return env->size-1;
}

struct Symbol* lookup(struct Env* env, char* id){
    // lookup through each environment until we find the id
    for(struct Env* tempenv = env; tempenv != NULL; tempenv = tempenv->prev){
        // search through the table by traversing the linked list
        for(struct Symbol* temp = env->head; temp!=NULL; temp = temp->next){
            if(temp->id == id){
                return temp;
            }
        }
    }
    // if the value is never reached
    return NULL;
}

void deleteItem(struct Env* env, char* id){
    // perform the same operations as a lookup, but then delete the entry
    // lookup through each environment until we find the id
    for(struct Env* tempenv = env; tempenv != NULL; tempenv = tempenv->prev){
        // search through the table by traversing the linked list
        struct Symbol* temp = env->head;
        for(int i=0; i<env->size; ++i){
            if(temp->id == id){
                // connect the previous and next symbols to each other
                if(temp->prev!=NULL) {
                    temp->prev->next = temp->next;
                } else {
                    //if the previous is equal to null, we have to set a new head
                    env->head = temp->next;
                }
                if(temp->next!=NULL){
                    temp->next->prev = temp->prev;
                } else {
                    //if the next is equal to null, we have to set a new tail
                    env->tail = temp->prev;
                }
                // delete the found symbol
                deleteSym(temp);
                env->size--;
                return;
            }
            temp = temp->next;
        }
    }

}

void printEnv(struct Env* env){
    printf("============Size: %i=============\n", env->size);
    struct Symbol* temp = env->head;
    for(int i=0; i<env->size; ++i){
        printf("====================================\n");
        printf("%s\t%s\t%s\n", temp->id, temp->type, temp->value);
        temp = temp->next;
    }
}

void updateVal(struct Symbol* sym, char* newval){
    free(sym->value);
    sym->value = newval;
}

// ======================= Environment =======================



#endif //SYMBOLTABLE_SYMBOLTABLE_H
