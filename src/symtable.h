
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
    const char *name;
    int id;
    void *functionData;
} symtableItem;

typedef struct symtable_s
{
	list *tables;
    list *scopes;
	ht_table_t *globalTable;
}symtable;

bool symtableEnterScope(symtable *table,char* scope);
symtable *symtableInit();