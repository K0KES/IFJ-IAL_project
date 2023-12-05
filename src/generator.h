#pragma once
#include "list.h"
#include "hashtable.h"
#include "string.h"

typedef struct generator_h
{
	list *functions; //List of strings that contains whole function blocks in assembly
    list *mainCode; //List of strings that contains whole 
    int counter; //Unique counter
    list *temporary;
    list *parserStack; 
    ht_table_t *functionCallsTable;
    void* table;
    bool substringGenerated;
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

char* generatorGenerateTempVarName(generator *gen);

char * concatString(int num_args, ...);

void printList(list *l);

char* stringToAssemblyStringFormat(char* inputString);
char * allocateString(char * string);

/*

1) push nazev_promene to parser stack
2) ondra pushne expression 
3) pop pop
4) generate

GF - Global frame


TF - Temporary frame


LF - Local frame (každá funkce bude mít svůj)




*/