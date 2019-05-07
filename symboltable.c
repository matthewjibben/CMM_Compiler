//
// Created by Matthew Jibben on 2/3/2019.
//
#include <string.h>
#include "symboltable.h"
//#include "cmm.tab.h"


// =======================   Symbol    =======================
void deleteSym(struct Symbol* sym){
//    free(sym->id);
//    free(sym->type);
//    free(sym->value);
    free(sym);
}
// =======================   Symbol    =======================


// ======================= Environment =======================
int envIDCount = 0;

Env* newEnvironment(struct Env* previous){
    Env* temp = malloc(sizeof(struct Env));
    if(temp==NULL){exit(EXIT_FAILURE);} // malloc error, exit
    //set the size to 0, it will be changed once we make insertions
    temp->size = 0;
    //The table currently has no elements, the array will be increased as needed
    temp->prev = previous;
    temp->head = temp->tail = NULL;
    temp->ID = envIDCount++;
    return temp;
}

int insertEntry(struct Env* env, Declaration* decl){
    struct Symbol* temp = malloc(sizeof(struct Symbol));
//    temp->id = id;
//    temp->type = type;
//    temp->value = value;
    temp->decl = decl;
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
//    printf("lookup start...\n");
    // lookup through each environment until we find the id
    for(struct Env* tempenv = env; tempenv != NULL; tempenv = tempenv->prev){
        //printf("new env\n");
        //printEnv(tempenv);
        // search through the table by traversing the linked list
        for(struct Symbol* temp = tempenv->head; temp!=NULL; temp = temp->next){
            //printf("|%s|\n", temp->decl->name);
            if(strcmp(temp->decl->name, id)==0){
                //printf("The value has been found\n");
                return temp;
            }
        }
    }

//    printf("The value has not been found\n");
    // if the value is never reached
    return NULL;
}

struct Symbol* lookupFunction(struct Env* env, char* id){
    struct Env* tempenv = env;
    // traverse backwards until we reach the global environment
    // all functions are stored in the global environment
    while(tempenv->type!=0){
        tempenv = tempenv->prev;
    }
    //now search through the environment and find a symbol with the same name and is a function
    // search through the table by traversing the linked list
    for(struct Symbol* temp = tempenv->head; temp!=NULL; temp = temp->next){
        //printf("|%s|\n", temp->decl->name);
        if(strcmp(temp->decl->name, id)==0 && temp->decl->type==FUNCTION){
            //printf("The value has been found\n");
            return temp;
        }
    }
}

struct Symbol* lookupCurrentEnv(struct Env* env, char* id){
    //same as looup, but only search through the current environment
    // search through the table by traversing the linked list
    for(struct Symbol* temp = env->head; temp!=NULL; temp = temp->next){
        if(strcmp(temp->decl->name, id)==0){
            return temp;
        }
    }
    //printf("The value has not been found\n");
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
        printf("============Size: %i ID: %i=============\n", env->size, env->ID);
        for (int i = 0; i < env->size; ++i) {
            printf("====================================\n");
            // print all necessary information here
            printIndent(0);
            if(temp->decl->name!=NULL) {
                printf("type: ");
                printTypeString(temp->decl->type);
                printf(" name: %s", temp->decl->name);
                if (temp->decl->isArray) {
                    printf("[%i]", temp->decl->size);
                    if (temp->decl->size != NULL) {
                        printf(" size: %i", temp->decl->size);
                    }
                } else if (temp->decl->type == FUNCTION) {
                    printParams(temp->decl->params);
                    printf("Param size: %i ", temp->decl->params->size);
                    printf("return: ");
                    printTypeString(temp->decl->returnType);
                }
                printf(" \n");
            }
            temp = temp->next;
        }
        printf("====================================\n");
    } else {
        printf("=================Empty Env ID: %i=================\n", env->ID);
        printf("====================================\n");
    }
}

void updateVal(struct Env* env, char* id, char* newval){        //todo update value expression?
    // use lookup to find if the id exists
    // if it does, delete the current value and replace it with the new one
    struct Symbol* sym = lookup(env, id);
    if(sym!=NULL) {
        //free(sym->value);
        //sym->value = newval;
    }
}

void updateType(struct Env* env, char* id, int newtype){
    // use lookup to find if the id exists
    // if it does, delete the current value and replace it with the new one
    struct Symbol* sym = lookup(env, id);
    if(sym!=NULL) {
        //free(sym->value);
        sym->decl->type = newtype;
    }
}

// takes as input an environment, and frees all necessary structs
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
    free(env);
}
// ======================= Environment =======================