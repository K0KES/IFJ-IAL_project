
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "custom_data_types/list.h"
#include "custom_data_types/hashtable.h"
#pragma once

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
    DATA_TYPE_UNDEFINED,
    DATA_TYPE_INTEGER,
    DATA_TYPE_FLOAT,
    DATA_TYPE_STRING
};

typedef struct{
    enum data_type type;
    char *name;
} functionArgument;

typedef struct{
    enum data_type returnType;
    list *arguments;
} functionData;

typedef struct{
    enum data_type type;
    const char *name;
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
}symtable;

///Initializes symtable - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
symtable *symtableInit();

///Enters scope with name
bool symtableEnterScope(symtable *table,char* scope);

///Exits current scope
void symtableExitScope(symtable *table);

///Free up all memory that was alocated by symtable
void symtableFree(symtable *table);

///Inserts new record into symtable and sets it as active (VAR or FUNCTION)
void symtableInsert(symtable *table, char *varName, bool isFunction);

///Prints all variables on current scope (DEBUG function)
void symtablePrintVariables(symtable *table);

///Sets active item variable type
void symtableSetVariableType(symtable *table, enum data_type type);

///Sets active item variable value (Must be called after symtableSetVariableType)
void symtableSetVariableValue(symtable *table, void* data);

///Inserts new argument into function arguments list
void symtableAddFunctionNextArgument(symtable *table);

///Sets data_type for last argument (Must be called after symtableFunctionNextArgument)
void symtableSetFunctionArgumentType(symtable *table, enum data_type type);

///Sets name for last argument (Must be called after symtableFunctionNextArgument)
void symtableSetFunctionArgumentName(symtable *table, char *name);

///Sets active item function return type
void symtableSetFunctionReturnType(symtable *table, enum data_type type);