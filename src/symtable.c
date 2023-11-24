#pragma once
#include "symtable.h"
#include <stdio.h>
#include "error.h"

symtable* symtableInit(generator *generator)
{
    symtable *table = (symtable *)(malloc(sizeof(symtable))); 
    if(table == NULL) return NULL;

    table->gen = generator;
    table->tables = listInit();
    table->scopes = listInit();
    table->functionCalls = listInit();

    table->functionCodeHeader = listInit();
    table->functionCodeBody = listInit();
    table->functionCodeFooter = listInit();

    if(table->tables == NULL){ free(table); return NULL; }
    
    if(!symtableEnterScope(table,NULL,NULL)){
        return NULL;
    }

    return table;
}

bool symtableEnterScope(symtable *table,char* scope,symtableItem *currentFunctionItem){
    printf("[SYMTABLE] Entering scope - %s \n",scope == NULL ? "GLOBAL" : scope);
    ht_table_t *hashmap;
    ht_init(&hashmap);
    if(hashmap == NULL) return false;

    listPushFirst(table->tables,hashmap);

    if(currentFunctionItem != NULL){
        table->currentFunction = currentFunctionItem;
    }

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

    for (size_t i = 0; i < HT_SIZE; i++)
    {
        ht_item_t *currentItem = (*hashmap)[i];

        while(currentItem != NULL){
            symtableItem * item = (symtableItem *)(currentItem->data);
            free(item->name);
            free(item->data);
            free(item->funcData);
            free(item);
            item = NULL;
            currentItem = currentItem->next;  
        }
    }
    
    //Need to loop through all symtableItems and free them with its data

    ht_delete_all(hashmap);
    free(hashmap);
    
    listPopFirst(table->tables);
    
    if(listLength(table->scopes) != 0){
        char *scopeString = (char *)listGetFirst(table->scopes);

        if(strcmp(scopeString,"while") != 0 && strcmp(scopeString,"if") != 0){
            char* line = listPopFirst(table->functionCodeHeader);
            
            while(line != NULL){
                listPushBack(table->gen->functions,line);
                line = listPopFirst(table->functionCodeHeader);
            }

            line = listPopFirst(table->functionCodeBody);
            while(line != NULL){
                listPushBack(table->gen->functions,line);
                line = listPopFirst(table->functionCodeBody);
            }

            line = listPopFirst(table->functionCodeFooter);
            while(line != NULL){
                listPushBack(table->gen->functions,line);
                line = listPopFirst(table->functionCodeFooter);
            }

            table->currentFunction = NULL;
        }

        printf("[SYMTABLE] Exiting scope - %s \n",scopeString);
        free(scopeString);
        listPopFirst(table->scopes);
    }else{
        printf("[SYMTABLE] Exiting scope - GLOBAL\n");
    }

}

void symtableFree(symtable *table){
    
    while(listLength(table->tables) != 0){
        symtableExitScope(table);
    }

    listDestroy(table->tables);
    listDestroy(table->scopes);
    listDestroy(table->functionCalls);

    freeContentOfListAndDestroy(table->functionCodeFooter);
    freeContentOfListAndDestroy(table->functionCodeBody);
    freeContentOfListAndDestroy(table->functionCodeHeader);
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
    newSymtableItem->type = DATA_TYPE_NOTSET;

    if(isFunction){
        newSymtableItem->funcData = (functionData *)malloc(sizeof(functionData));
        newSymtableItem->funcData->returnType = DATA_TYPE_NOTSET;
        newSymtableItem->funcData->arguments = listInit();
    }

    ht_insert(currentTable,string,newSymtableItem);

    table->activeItem = newSymtableItem;

    table->gen->counter++;
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
                printf("- %s (VAR, TYPE: %d) = ",item->name,item->type);
                if(item->type == DATA_TYPE_NOTSET) printf("NOT SET\n");
                if(item->type == DATA_TYPE_DOUBLE) printf("%lf\n",*((double *)item->data));
                if(item->type == DATA_TYPE_INTEGER) printf("%d\n",*((int *)item->data));
                if(item->type == DATA_TYPE_STRING) printf("%s\n",*((char **)item->data));
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

void symtableSetDataType(symtable *table, enum data_type type, bool nullable){
    if(table->activeItem == NULL) return;
    
    if(table->activeItem->funcData == NULL){
        table->activeItem->type = type;
        table->activeItem->nullable = nullable;
    }else{
        
        if(table->activeItem->funcData->endOfArguments == false){
            functionArgument *argument = (functionArgument *)listGetLast(table->activeItem->funcData->arguments);
            argument->type = type;
            argument->nullable = nullable;
            
        }else{
            table->activeItem->funcData->returnType = type;
            table->activeItem->funcData->returnTypeNullable = nullable;
            
        }
    }
}

void symtableSetVariableValue(symtable *table, void* data){
    if(table->activeItem == NULL) return;
    if(table->activeItem->type == DATA_TYPE_NOTSET || table->activeItem->type == DATA_TYPE_VOID) return;

    if(table->activeItem->type == DATA_TYPE_DOUBLE){
        double *newSpace = (double *)malloc(sizeof(double));
        memcpy(newSpace,(double *)data,sizeof(double));

        table->activeItem->data = newSpace;
    }

    if(table->activeItem->type == DATA_TYPE_INTEGER){
        int *newSpace = (int *)malloc(sizeof(int));
        memcpy(newSpace,(int *)data,sizeof(int));

        table->activeItem->data = newSpace;
    }

    if(table->activeItem->type == DATA_TYPE_STRING){
        int stringLength = strlen((char *)data) + 1;
        char *string = (char *)malloc(stringLength);
        memcpy(string,(char *)data,stringLength);

        table->activeItem->data = string;
    }
}

void symtableAddFunctionNextArgument(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    functionArgument *argument = (functionArgument *)malloc(sizeof(functionArgument));
    argument->type = DATA_TYPE_NOTSET;
    argument->name = "_";
    argument->id = "";
    argument->nullable = false;
    
    listPushBack(table->activeItem->funcData->arguments,argument);
}
/*
void symtableSetFunctionArgumentType(symtable *table, enum data_type type){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    functionArgument *argument = (functionArgument *)listGetLast(table->activeItem->funcData->arguments);
    argument->type = type;
}*/

void symtableSetFunctionArgumentID(symtable *table, char *id){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    int stringLength = strlen(id) + 1;
    char *string = (char *)malloc(stringLength);
    memcpy(string,id,stringLength);

    functionArgument *argument = (functionArgument *)listGetLast(table->activeItem->funcData->arguments);
    argument->id = string;
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

/*
void symtableSetFunctionReturnType(symtable *table, enum data_type type){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    table->activeItem->funcData->returnType = type;
}*/

void symtableFunctionEndOfArguments(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    table->activeItem->funcData->endOfArguments = true;

    printf("----------MANUAL ENTER SCOPE-----------\n");
    symtableEnterScope(table,table->activeItem->name,table->activeItem);   

    symtableCreateFunctionStructure(table);
}

enum data_type symtableGetReturnTypeOfCurrentScope(symtable *table){
    if(table->currentFunction == NULL) return DATA_TYPE_VOID;
    return table->currentFunction->funcData->returnType;
}

symtableItem *symtableFindSymtableItem(symtable *table, char *varName){
    listNode *currentNode = table->tables->first;
    while(currentNode != NULL){
        ht_table_t *currentTable = (ht_table_t *)(currentNode->data);
        ht_item_t *item = ht_search(currentTable,varName);
        if(item != NULL){
            return (symtableItem *)(item->data);
        }
        currentNode = currentNode->next;
    }   
    return NULL;
}

bool symtableIsVariableDefined(symtable *table,char *varName){
    return symtableFindSymtableItem(table,varName) != NULL;
}

bool symtableIsVariableInitiated(symtable *table,char *varName){
    symtableItem *item = symtableFindSymtableItem(table,varName);
    if(item == NULL) return false;
    return item->data != NULL;
}

bool symtableIsActiveVariableInitiated(symtable *table){
    if(table == NULL) return false;
    if(table->activeItem == NULL) return false;
    return table->activeItem->data != NULL;
}

enum data_type symtableGetVariableType(symtable *table, char *varName){
    symtableItem *item = symtableFindSymtableItem(table,varName);

    if(item == NULL) raiseError(ERR_UNDEFINED_VARIABLE);
    if(item->funcData == NULL){
        return item->type;
    }else{
        return item->funcData->returnType;
    }
}

void symtableCreateFunctionStructure(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    printf("-------------->>> CREATING STRUCTURE CODE \n");

    generatorPushStringToList(table->functionCodeHeader,concatString(2,"LABEL $",table->activeItem->name));
    generatorPushStringToList(table->functionCodeHeader,"PUSHFRAME");
    generatorPushStringToList(table->functionCodeHeader,"DEFVAR LF@%retval");
    generatorPushStringToList(table->functionCodeHeader,"MOVE LF@%retval nil@nil");

    listNode *arg = table->activeItem->funcData->arguments->first;
    int i = 1;
    while(arg != NULL){
        char str[128];
        char str2[128];
        sprintf(str, "LF@param%d", i);
        sprintf(str2, "LF@!%d",i);
        generatorPushStringToList(table->functionCodeHeader,concatString(2,"DEFVAR ",str));
        generatorPushStringToList(table->functionCodeHeader,concatString(4,"MOVE ",str," ",str2));
        i++;
        arg = arg->next;
    }

    generatorPushStringToList(table->functionCodeFooter,"POPFRAME");
    generatorPushStringToList(table->functionCodeFooter,"RETURN");
}

void symtablePushCode(symtable *table, char* code){
    if(table == NULL) return;
    if(listLength(table->scopes) == 0){
        generatorPushStringToList(table->gen->mainCode,code);
    }else{
        //Check if we are in function
        if(table->currentFunction == NULL){
            generatorPushStringToList(table->gen->mainCode,code);
        }else{
            generatorPushStringToList(table->functionCodeBody,code);
        }
    }
    
}

char* symtableGetScopePrefixName(symtable *table){
    if(listLength(table->scopes) != 0){
        char *scopeString = (char *)listGetFirst(table->scopes);

        char str[64];
        sprintf(str,"%d",table->gen->counter);
        
        return concatString(3,scopeString,str,"_");
    }else{
        return "global_";
    }
}

char* symtableGetVariablePrefix(symtable *table){
    return concatString(2,symtableGetFramePrefix(table),symtableGetScopePrefixName(table));
}

char* symtableGetFramePrefix(symtable *table){
    if(listLength(table->scopes) != 0){
        return "LF@";
    }else{
        return "GF@";
    }
}

//FUNCTION CALLS ex. foo(par1, par2, par3);

void symtableFunctionCallStart(symtable *table, char *funcName){
    table->lastFunctionCall = funcName;
    
    functionData *funcData = (functionData *)(malloc(sizeof(functionData)));
    funcData->returnType = DATA_TYPE_NOTSET;
    funcData->arguments = listInit();

    listPushFirst(table->functionCalls,funcData);
}

void symtableFunctionCallNextParameter(symtable *table){
    functionData *funcData = (functionData *)listGetLast(table->functionCalls);
    
    functionArgument *argument = (functionArgument *)malloc(sizeof(functionArgument));
    argument->type = DATA_TYPE_NOTSET;
    argument->name = "_";
    argument->id = "";
    argument->nullable = false;
    listPushBack(funcData->arguments,argument);
}

void symtableFunctionCallSetParameterType(symtable *table, enum data_type type, bool nullable){
    functionData *funcData = (functionData *)listGetLast(table->functionCalls);
    functionArgument *argument = (functionArgument *)listGetLast(funcData->arguments);
    argument->type = type;
    argument->nullable = nullable;
}

void symtableFunctionCallSetParameterName(symtable *table, char* name){
    functionData *funcData = (functionData *)listGetLast(table->functionCalls);
    functionArgument *argument = (functionArgument *)listGetLast(funcData->arguments);

    int stringLength = strlen(name) + 1;
    char *string = (char *)malloc(stringLength);
    memcpy(string,name,stringLength);

    argument->name = string;
}

void symtableFunctionCallEnd(symtable *table){
    functionData *funcData = (functionData *)listGetLast(table->functionCalls);

    symtableItem *item = symtableFindSymtableItem(table,table->lastFunctionCall);
    if(item != NULL){
        if(item->funcData == NULL) raiseError(ERR_UNDEFINED_FUNCTION);

        listNode *callCurrentItem = funcData->arguments->first;
        listNode *funcCurrentItem = item->funcData->arguments->first;

        while(true){
            if(callCurrentItem == NULL && funcCurrentItem != NULL) raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            if(callCurrentItem != NULL && funcCurrentItem == NULL) raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            if(callCurrentItem == NULL && funcCurrentItem == NULL) break;
            functionArgument *callArg = (functionArgument *)(callCurrentItem->data);
            functionArgument *funcArg = (functionArgument *)(funcCurrentItem->data);
            
            if(callArg->type != funcArg->type){
                raiseError(ERR_WRONG_TYPE);
            }
            if(strcmp(callArg->name,funcArg->name) != 0){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            if(callArg->nullable != funcArg->nullable){
                if(!(funcArg->nullable && !callArg->nullable)){
                    raiseError(ERR_WRONG_TYPE);
                }
            }

            callCurrentItem = callCurrentItem->next;
            funcCurrentItem = funcCurrentItem->next;
        }

        listDestroy(funcData->arguments);
        free(funcData);
        funcData = NULL;
        listPopLast(table->functionCalls);
    }
}

void symtableSetActiveItem(symtable *table, char* varName){
    if(table == NULL) return;
    symtableItem *item = symtableFindSymtableItem(table,varName);
    if(item == NULL) return;
    table->activeItem = item;
}

enum data_type symtableGetActiveItemType(symtable *table){
    if(table == NULL) return DATA_TYPE_NOTSET;
    if(table->activeItem == NULL) return DATA_TYPE_NOTSET;
    return table->activeItem->type;
}

char *symtableGetActiveItemName(symtable *table){
    if(table == NULL) return NULL;
    if(table->activeItem == NULL) return NULL;
    return table->activeItem->name;
}

void symtableSameTypes(enum data_type type1,enum data_type type2){
    if(type1 == type2) {return;}
    raiseError(ERR_WRONG_TYPE);
}