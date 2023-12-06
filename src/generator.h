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

///@brief Initializes generator - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
generator *generatorInit();

/// @brief Generatates output to output file
/// @param gen pointer to generator
void generatorGenerateOutput(generator *gen);

/// @brief Free 
/// @param gen pointer to generator
void generatorFree(generator *gen);

/// @brief Pops first string from list 
/// @param list pointer to list
/// @return char*
char* generatorPopFirstStringFromList(list *list);

/// @brief Pops last string from list 
/// @param list pointer to list
/// @return char*
char* generatorPopLastStringFromList(list *list);

/// @brief Get first string from list 
/// @param list pointer to list
/// @return char*
char* generatorGetFirstStringFromList(list *list);

/// @brief Get last string from list 
/// @param list pointer to list
/// @return char*
char* generatorGetLastStringFromList(list *list);

/// @brief Push first string from list 
/// @param list pointer to list
/// @param char* string
void generatorPushStringFirstToList(list *list, char *string);

/// @brief Push last string from list 
/// @param list pointer to list
/// @param char* string
void generatorPushStringToList(list *list, char *string);

/// @brief Generates random temp name ex. temp7
/// @param gen pointer to generator
/// @return char*
char* generatorGenerateTempVarName(generator *gen);

/// @brief Concats string
/// @param num_args number of args
/// @param anystrings
/// @return char*
char * concatString(int num_args, ...);

/// @brief Debug prints list
/// @param l pointer to list
void printList(list *l);

/// @brief Converts string to assembly string format
/// @param inputString string
/// @return assembly string
char* stringToAssemblyStringFormat(char* inputString);

/// @brief Deep copy string
/// @param string string to copy
/// @return string
char * allocateString(char * string);

/// @brief Creates substring definition in assembly
/// @param gen pointer to gen
void generatorAddSubstringFunction(generator *gen);