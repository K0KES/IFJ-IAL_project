#include "custom_data_types/list.h"
#include "custom_data_types/hashtable.h"
#pragma once

typedef struct generator_h
{
	list *functions; //List of strings that contains whole function blocks in assembly
    list *mainCode; //List of strings that contains whole 
    int counter; //Unique counter
    list *temporary;
    list *parserStack; 
}generator;


///Initializes generator - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
generator *generatorInit();

void generatorGenerateOutput(generator *gen);

///Free up all memory that was alocated by symtable
void generatorFree(generator *gen);

char* generatorPopFirstStringFromList(list *list);

char* generatorPopLastStringFromList(list *list);

char* generatorGetFirstStringFromList(list *list);

char* generatorGetLastStringFromList(list *list);

void generatorPushStringFirstToList(list *list, char *string);

void generatorPushStringToList(list *list, char *string);

char* generatorGenerateTempVarName();

char * concatString(int num_args, ...);

void printList(list *l);

/*

1) push nazev_promene to parser stack
2) ondra pushne expression 
3) pop pop
4) generate

GF - Global frame


TF - Temporary frame


LF - Local frame (každá funkce bude mít svůj)




*/