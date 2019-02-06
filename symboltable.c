//
// Created by Matthew Jibben on 2/3/2019.
//
#include "symboltable.h"
//#include "cmm.tab.h"


// =======================   Symbol    =======================
void deleteSym(struct Symbol* sym){
    free(sym->id);
    free(sym->type);
    free(sym->value);
    free(sym);
}
// =======================   Symbol    =======================


// ======================= Environment =======================
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
    // lookup a symbol, then delete the entry
    struct Symbol* temp = lookup(env, id);
    if(temp!=NULL){
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
    }

}

void printEnv(struct Env* env){
    struct Symbol* temp = env->head;
    if(temp!=NULL) {
        printf("============Size: %i=============\n", env->size);
        for (int i = 0; i < env->size; ++i) {
            printf("====================================\n");
            printf("%s\t%s\t%s\n", temp->id, temp->type, temp->value);
            temp = temp->next;
        }
        printf("====================================\n");
    } else {
        printf("=================Empty Env=================\n");
        printf("====================================\n");
    }
}

void updateVal(struct Env* env, char* id, char* newval){
    // use lookup to find if the id exists
    // if it does, delete the current value and replace it with the new one
    struct Symbol* sym = lookup(env, id);
    if(sym!=NULL) {
        free(sym->value);
        sym->value = newval;
    }
}

void updateType(struct Env* env, char* id, char* newtype){
    // use lookup to find if the id exists
    // if it does, delete the current value and replace it with the new one
    struct Symbol* sym = lookup(env, id);
    if(sym!=NULL) {
        free(sym->value);
        sym->type = newtype;
    }
}

// takes as input an environment, and frees all variables held in it
// this is used when a scope is completed
void freeEnv(struct Env* env){
    // search through the table by traversing the linked list
    struct Symbol* temp;
    while(env->head!=NULL){
        temp = env->head;
        env->head = env->head->next;
        deleteSym(temp);
    }
    env->size = 0;
    //free(env);
}
// ======================= Environment =======================