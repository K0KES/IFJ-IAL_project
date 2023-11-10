
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
 * @brief enum for symbol types
*/
typedef enum {
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_FUNCTION
} symbolType;

/**
 * @brief enum for data types
*/
typedef enum {
    DATA_TYPE_UNDEFINED,
    DATA_TYPE_INTEGER,
    DATA_TYPE_FLOAT,
    DATA_TYPE_STRING
} data_type_t;

typedef struct symtable_s
{
	list *tables;
    list *scopes;
	ht_table_t *globalTable;
}symtable;

bool symtableEnterScope(symtable *table,char* scope);
void symtableInit(symtable *table);