
#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "list.h"
#include "hashtable.h"
#include "generator.h"
#include "error.h"

/**
 * @file sym_table.h
 * @author Vít Slavíček
 * @brief Symbol table header
 * @date 9.10.2023
 * 
*/

/**
 * @brief enum for data types
*/
enum data_type{
    DATA_TYPE_INTEGER = 120,
    DATA_TYPE_DOUBLE = 121,
    DATA_TYPE_STRING = 122,
    DATA_TYPE_BOOL = 123,
    DATA_TYPE_VOID = 124,
    DATA_TYPE_NOTSET = 125
};

typedef struct{
    enum data_type type;
    bool nullable;
    char *name;
    char *id;
} functionArgument;

typedef struct{
    enum data_type returnType;
    bool returnTypeNullable;
    list *arguments;
    bool endOfArguments;
    char *callName;
} functionData;

typedef struct{
    enum data_type type;
    bool nullable;
    char *name;
    void* data;
    int id;
    functionData *funcData;
} symtableItem;

typedef struct symtable_s
{
	list *tables;
    list *scopes;
	ht_table_t *globalTable;
    symtableItem *activeItem;
    symtableItem *currentFunction;
    list *functionCalls;
    char* lastFunctionCall;
    generator *gen;
    list *functionCodeHeader; 
    list *functionCodeBody;
    list *functionCodeFooter;
}symtable;

///Initializes symtable - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
symtable *symtableInit();

///Enters scope with name
bool symtableEnterScope(symtable *table,char* scope,symtableItem *currentFunctionItem);

///Exits current scope
void symtableExitScope(symtable *table);

///Free up all memory that was allocated by symtable
void symtableFree(symtable *table);

///Inserts new record into symtable and sets it as active (VAR or FUNCTION)
void symtableInsert(symtable *table, char *varName, bool isFunction);

///Prints all variables on current scope (DEBUG function)
void symtablePrintVariables(symtable *table);

///Sets active item variable type or function argument type or function return type
void symtableSetDataType(symtable *table, enum data_type type, bool nullable);

///Sets active item variable value (Must be called after symtableSetVariableType)
void symtableSetVariableValue(symtable *table, void* data);

///Inserts new argument into function arguments list
void symtableAddFunctionNextArgument(symtable *table);

///Sets name for last argument (Must be called after symtableFunctionNextArgument)
void symtableSetFunctionArgumentName(symtable *table, char *name);

symtableItem *symtableFindSymtableItem(symtable *table, char *varName);
bool symtableIsVariableDefined(symtable *table,char *varName);
bool symtableIsVariableInitiated(symtable *table,char *varName);
bool symtableIsActiveVariableInitiated(symtable *table);
enum data_type symtableGetVariableType(symtable *table, char *varName);
void symtableFunctionCallStart(symtable *table, char *funcName);
void symtableFunctionCallNextParameter(symtable *table);
void symtableFunctionCallSetParameterType(symtable *table, enum data_type type, bool nullable);
void symtableFunctionCallSetParameterName(symtable *table, char* name);
//void symtableFunctionCallEnd(symtable *table);
void symtableEndOfFile(symtable *table);
void symtableCreateFunctionStructure(symtable *table);
void symtableFunctionEndOfArguments(symtable *table);
void symtableSetFunctionArgumentID(symtable *table, char *id);
enum data_type symtableGetReturnTypeOfCurrentScope(symtable *table);

char* symtableGetFramePrefix(symtable *table, char *varName);
char* symtableGetVariablePrefix(symtable *table, char *varName);
char* symtableGetScopePrefixName(symtable *table);

void symtablePushCode(symtable *table, char* code);

char *symtableGetActiveItemName(symtable *table);
void symtableSameTypes(enum data_type type1,enum data_type type2);

void symtableSetFunctionCallName(symtable *table, char* varName);
///Sets data_type for last argument (Must be called after symtableFunctionNextArgument)
//void symtableSetFunctionArgumentType(symtable *table, enum data_type type);

///Sets active item function return type
//void symtableSetFunctionReturnType(symtable *table, enum data_type type);