
#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "custom_data_types/list.h"
#include "custom_data_types/hashtable.h"

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
    DATA_TYPE_NOTSET,
    DATA_TYPE_INTEGER,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
    DATA_TYPE_VOID
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
}symtable;

///Initializes symtable - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
symtable *symtableInit();

///Enters scope with name
bool symtableEnterScope(symtable *table,char* scope,symtableItem *currentFunctionItem);

///Exits current scope
void symtableExitScope(symtable *table);

///Free up all memory that was alocated by symtable
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

///Sets data_type for last argument (Must be called after symtableFunctionNextArgument)
//void symtableSetFunctionArgumentType(symtable *table, enum data_type type);

///Sets active item function return type
//void symtableSetFunctionReturnType(symtable *table, enum data_type type);