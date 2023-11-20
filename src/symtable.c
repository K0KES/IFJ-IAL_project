#include "symtable.h"
#include "scanner.h"
#include <stdio.h>


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
    newSymtableItem->funcData = NULL;
    newSymtableItem->data = NULL;

    if(isFunction){
        newSymtableItem->funcData = (functionData *)malloc(sizeof(functionData));
        newSymtableItem->funcData->returnType = DATA_TYPE_UNDEFINED;
        newSymtableItem->funcData->arguments = listInit();
    }

    ht_insert(currentTable,string,newSymtableItem);

    table->activeItem = newSymtableItem;
}

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
            
            if(item->funcData == NULL){
                printf("- %s (VAR, TYPE: %d) = %d \n",item->name,item->type,item->data);
            }else{
                printf("- %s (FUNC, RETURN TYPE: %d) - ",item->name,item->funcData->returnType);
                bool noArgs = false;
                functionArgument *argument = (functionArgument *)listPopFirst(item->funcData->arguments);
                if(argument == NULL){
                    printf("NO ARGS\n");
                    noArgs = true;
                }
                while(argument != NULL){
                    printf("%s (TYPE: %d) ",argument->name,argument->type);
                    argument = (functionArgument *)listPopFirst(item->funcData->arguments);
                }
                if(!noArgs){
                    printf("\n");
                }
            }
            
            currentItem = currentItem->next;
        }
    }
}

/*

ASI NEBUDOU POTŘEBA => SEMANTICKE CHYBY BUDOU UZ ZDE

bool symtableContainsSymbol(symtable *table, char* name){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);
    symtableItem* item = (symtableItem *)ht_get(currentTable,name);
    if(item == NULL) return false;
    return (item->funcData == NULL);
}

Returns true if current scope contains function with corresponding name
bool symtableContainsFunction(symtable *table, char* name){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);
    symtableItem* item = (symtableItem *)ht_get(currentTable,name);
    if(item == NULL) return false;
    return (item->funcData != NULL);
}

Returns true if current scope contains variable with corresponding name
bool symtableContainsVariable(symtable *table, char* name){
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);
    symtableItem* item = (symtableItem *)ht_get(currentTable,name);
    return item != NULL;
}*/  

void symtableSetVariableType(symtable *table, enum data_type type){
    if(table->activeItem == NULL) return;
    table->activeItem->type = type;
}

void symtableSetVariableValue(symtable *table, void* data){
    if(table->activeItem == NULL) return;

    if(table->activeItem->type == DATA_TYPE_FLOAT){
        float* floatData = (float *)data;
        float *newSpace = (float *)malloc(sizeof(float));
        memcpy(newSpace,floatData,sizeof(float));

        table->activeItem->data = newSpace;
    }
}

void symtableAddFunctionNextArgument(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    functionArgument *argument = (functionArgument *)malloc(sizeof(functionArgument));
    
    listPushBack(table->activeItem->funcData->arguments,argument);
}

void symtableSetFunctionArgumentType(symtable *table, enum data_type type){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    functionArgument *argument = (functionArgument *)listGetLast(table->activeItem->funcData->arguments);
    argument->type = type;
}

void symtableSetFunctionArgumentName(symtable *table, char *name){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    int stringLength = strlen(name) + 1;
    char *string = (char *)malloc(stringLength);
    memcpy(string,name,stringLength);

    functionArgument *argument = (functionArgument *)listGetLast(table->activeItem->funcData->arguments);
    argument->name = string;
}

void symtableSetFunctionReturnType(symtable *table, enum data_type type){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    table->activeItem->funcData->returnType = type;
}

/*
symtableSetActiveToken(symtable *table, token token, bool isFunction) -> EDIT Insert
symtableSetVariableType()
symtaleSetFunctionParams()

symtableFunctionCallInitiated(symtable *table, ...)
symtableFunctionCallNextArgument(symtable *table, ...)
symtableFunctionCallExterminate(symtable *table, ...)

symtableCompareFunctionReturnType

*/