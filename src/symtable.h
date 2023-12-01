/**
 * @file symtable.h
 * @author Vít Slavíček
 * @brief Symtable header
 * @date 9.10.2023
*/

#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "list.h"
#include "hashtable.h"
#include "generator.h"
#include "error.h"

/// @brief Enum for symtable item data types
/// We started values from 120 to match TOKEN types
enum data_type{
    DATA_TYPE_INTEGER = 120,
    DATA_TYPE_DOUBLE = 121,
    DATA_TYPE_STRING = 122,
    DATA_TYPE_BOOL = 123,
    DATA_TYPE_VOID = 124,
    DATA_TYPE_NOTSET = 125
};

/// @brief Structure that holds function argument or paramer from callee
typedef struct{
    enum data_type type;
    bool nullable;
    char *name;
    char *id;
} functionArgument;

/// @brief Structure that holds entire function definition data or function callee data
typedef struct{
    enum data_type returnType;
    bool returnTypeNullable;
    list *arguments;
    bool endOfArguments;
    char *callName;
} functionData;

/// @brief Structure of generic symtableItem (function or variable)
typedef struct{
    enum data_type type;
    bool nullable;
    bool isConstant;
    bool valueIsSet;
    char *name;
    int id;
    functionData *funcData;
} symtableItem;

/// @brief Structure of entire symtable
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

/// @brief Initializes symtable - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
symtable *symtableInit();

/// @brief Enters scope with name
/// @param table Pointer to symtable
/// @param currentFunctionItem If we are entering to function set it to function symtableItem else NULL
bool symtableEnterScope(symtable *table,char* scope,symtableItem *currentFunctionItem);

/// @brief Exits current scope
/// @param table Pointer to symtable
void symtableExitScope(symtable *table);

/// @brief Free up all memory that was allocated by symtable
/// @param table Pointer to symtable
void symtableFree(symtable *table);

/// @brief Inserts new record into symtable and sets it as active (VAR or FUNCTION)
/// @param table Pointer to symtable
/// @param varName Variable name
/// @param isFunction Is it function?
void symtableInsert(symtable *table, char *varName, bool isFunction);

///@brief Prints all variables on current scope (DEBUG function)
/// @param table Pointer to symtable
void symtablePrintVariables(symtable *table);

///@brief Sets active item variable type or function argument type or function return type
/// @param table Pointer to symtable
/// @param type Data type of variable
/// @param nullable Can be variable nullable?
void symtableSetDataType(symtable *table, enum data_type type, bool nullable);

/// @brief Sets active item variable value (Must be called after symtableSetVariableType)
/// @param table Pointer to symtable
void symtableSetVariableValue(symtable *table);

/// @brief Returns symtableItem in current scope or above if not found returns NULL
/// @param table Pointer to symtable
/// @param varName Variable name to find
/// @return symtableItem or NULL
symtableItem *symtableFindSymtableItem(symtable *table, char *varName);

/// @brief Returns true if variable is defined in current scope or above else false
/// @param table Pointer to symtable
/// @param varName Variable name to check
/// @return true or false
bool symtableIsVariableDefined(symtable *table,char *varName);

/// @brief Returns true if variable is initiated in current scope or above else false
/// @param table Pointer to symtable
/// @param varName Variable name to check
/// @return true or false
bool symtableIsVariableInitiated(symtable *table,char *varName);

/// @brief Returns true if active item is initiated in current scope or above else false
/// @param table Pointer to symtable
/// @return true or false
bool symtableIsActiveVariableInitiated(symtable *table);

/// @brief Returns data_type of variable or DATA_TYPE_NOTSET if not found
/// @param table Pointer to symtable
/// @param varName Variable name to check
/// @return enum data_type
enum data_type symtableGetVariableType(symtable *table, char *varName);

/// @brief Returns nullable of variable type or nullable of function return type
/// @param table Pointer to symtable
/// @param varName Variable name
/// @return true or false
bool symtableGetVariableNullable(symtable *table, char *varName);

/// @brief Tells symtable you are about to do function call ex. foo(x)
/// @param table Pointer to symtable
/// @param funcName Function name
void symtableFunctionCallStart(symtable *table, char *funcName);

/// @brief Sets lastFunctionCall to funcName
/// @param table Pointer to symtable
/// @param funcName Function name
void symtableSetFunctionCallName(symtable *table, char* funcName);

/// @brief Initalizes space for next parameter in function call
/// @param table Pointer to symtable
void symtableFunctionCallNextParameter(symtable *table);

/// @brief Sets current function call parameter type
/// @param table Pointer to symtable
/// @param type Data type
/// @param nullable Is parameter type nullable
void symtableFunctionCallSetParameterType(symtable *table, enum data_type type, bool nullable);

/// @brief Sets current function call parameter name
/// @param table Pointer to symtable
/// @param name Parameter name
void symtableFunctionCallSetParameterName(symtable *table, char* name);

/// @brief Initalizes space for next argument in function definiton
/// @param table Pointer to symtable
void symtableAddFunctionNextArgument(symtable *table);

/// @brief Sets current function definition argument name
/// @param table Pointer to symtable
/// @param name Parameter name
void symtableSetFunctionArgumentName(symtable *table, char *name);

/// @brief Sets current function definition argument id
/// @param table Pointer to symtable
/// @param name Parameter id
void symtableSetFunctionArgumentID(symtable *table, char *id);

/// @brief Tells symtable it is end of function argument definiton
/// @param table Pointer to symtable
void symtableFunctionEndOfArguments(symtable *table);

/// @brief Create assembly function structure and appends it to generator
/// @param table Pointer to symtable
void symtableCreateFunctionStructure(symtable *table);

/// @brief Get frame prefix GF@ or LF@ of variable
/// @param table Pointer to symtable
/// @param varName Variable name
/// @return GF@ or LF@
char* symtableGetFramePrefix(symtable *table, char *varName);

/// @brief Get variable prefix of variable
/// @param table Pointer to symtable
/// @param varName Variable name
/// @return GF@global_ or for ex. LF@myfunc_
char* symtableGetVariablePrefix(symtable *table, char *varName);

/// @brief Get current scope prefix
/// @param table Pointer to symtable
/// @return global_ or for ex. myfunc_
char* symtableGetScopePrefixName(symtable *table);

/// @brief Get current scope return type
/// @param table Pointer to symtable
/// @return data_type
enum data_type symtableGetReturnTypeOfCurrentScope(symtable *table);

/// @brief Appends code to appropriate list in generator
/// @param table Pointer to symtable
/// @param code Code string
void symtablePushCode(symtable *table, char* code);

/// @brief Manually sets active item
/// @param table Pointer to symtable
/// @param varName Variable name
void symtableSetActiveItem(symtable *table, char* varName);

/// @brief Gets active item name
/// @param table Pointer to symtable
/// @return string
char *symtableGetActiveItemName(symtable *table);

/// @brief Checks if types are same else raise error
/// @param type1 Type 1 to check
/// @param type2 Type 2 to check
void symtableCheckSameTypes(enum data_type type1,enum data_type type2);

/// @brief This function is called on the end of file and it does sematic checks
/// @param table Pointer to symtable
void symtableEndOfFile(symtable *table);