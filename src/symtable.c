#include "symtable.h"
#include "scanner.h"
#include <stdio.h>

/*
Initializes symtable - MUST BE CALLED BEFORE ANY OTHER FUNCTION!
*/
symtable* symtableInit(){
    symtable *table = (symtable *)(malloc(sizeof(symtable))); 
    if(table == NULL) return NULL;

    table->tables = listInit();
    table->scopes = listInit();

    if(table->tables == NULL){ free(table); return NULL; }
    
    if(!symtableEnterScope(table,NULL)){
        return NULL;
    }

    return table;
}

/*
Enters scope with name and allocates memory
*/
bool symtableEnterScope(symtable *table,char* scope){
    printf("Entering scope - %s \n",scope == NULL ? "GLOBAL" : scope);
    ht_table_t *hashmap;
    ht_init(&hashmap);
    if(hashmap == NULL) return false;
    

    listPushFirst(table->tables,hashmap);

    if(scope != NULL){
        int stringLength = strlen(scope) + 1;
        char *string = (char *)malloc(stringLength);
        memcpy(string,scope,stringLength);

        listPushFirst(table->scopes,string);
    }

    
}

/*
Exits current scope and free ups scope memory
*/
void symtableExitScope(symtable *table){
    
    ht_table_t *hashmap;

    hashmap = (ht_table_t *)listGetFirst(table->tables);

    /*
    printf("LOADED ADRESS: %d \n",hashmap);

    float *val = ht_get(hashmap,"test123");

    printf("READED VALUE %f \n", *val);*/

    //TODO - Free every symtable item on exit (64B leak)
    
    ht_delete_all(hashmap);
    free(hashmap);
    
    listPopFirst(table->tables);
    
    if(listLength(table->scopes) != 0){
        char *scopeString = (char *)listGetFirst(table->scopes);
        printf("Exiting scope - %s \n",scopeString);
        free(scopeString);
        listPopFirst(table->scopes);
    }else{
        printf("Exiting scope - GLOBAL\n");
    }

}

/*
Free up all memory that was alocated by symtable
*/
void symtableFree(symtable *table){
    
    while(listLength(table->tables) != 0){
        symtableExitScope(table);
    }

    listDestroy(table->tables);
    listDestroy(table->scopes);
    free(table);
}

void symtableInsert(symtable *table, char *varName, bool isFunction){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);

    int stringLength = strlen(varName) + 1;
    char *string = (char *)malloc(stringLength);
    memcpy(string,varName,stringLength);
    
    symtableItem *newSymtableItem = (symtableItem *)malloc(sizeof(symtableItem));
    newSymtableItem->name = string;

    ht_insert(currentTable,string,newSymtableItem);
}

/*
Prints all variables on current scope (DEBUG function)
*/
void symtablePrintVariables(symtable *table){
    if(listLength(table->scopes) != 0){
        char *currentScope = (char *)listGetFirst(table->scopes);
        printf("Variables on scope - %s \n",currentScope);
    }else{
        printf("Variables on scope - GLOBAL \n");
    }
    
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);

    for (size_t i = 0; i < HT_SIZE; i++)
    {
        ht_item_t *currentItem = (*currentTable)[i];
        while(currentItem != NULL){
            symtableItem *item = (symtableItem *)(currentItem->data);
            printf("- %s \n",item->name);
            currentItem = currentItem->next;
        }
    }
}

/*
Returns true if current scope contains symbol with corresponding name
*/
bool symtableContainsSymbol(symtable *table, char* name){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);
    symtableItem* item = (symtableItem *)ht_get(currentTable,name);
    if(item == NULL) return false;
    return (item->functionData == NULL);
}

/*
Returns true if current scope contains function with corresponding name
*/
bool symtableContainsFunction(symtable *table, char* name){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);
    symtableItem* item = (symtableItem *)ht_get(currentTable,name);
    if(item == NULL) return false;
    return (item->functionData != NULL);
}

/*
Returns true if current scope contains variable with corresponding name
*/
bool symtableContainsVariable(symtable *table, char* name){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);
    symtableItem* item = (symtableItem *)ht_get(currentTable,name);
    return item != NULL;
}   



/*
symtableSetActiveToken(symtable *table, token token, bool isFunction) -> EDIT Insert
symtableSetVariableType()
symtaleSetFunctionParams()
symtableSetFunctionReturnType()
symtableUpdate(symtable *table, value)

symtableFunctionCallInitiated(symtable *table, ...)
symtableFunctionCallNextArgument(symtable *table, ...)
symtableFunctionCallExterminate(symtable *table, ...)

symtableCompareFunctionReturnType

*/