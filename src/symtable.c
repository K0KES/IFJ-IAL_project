/**
 * @file symtable.c
 * @author Vít Slavíček
 * @brief Symtable
 * @date 9.10.2023
*/

#pragma once
#include "symtable.h"
#include <stdio.h>
#include "error.h"
#include "generator.h"

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
    DEBUG_PRINTF("[Symtable] Entering scope - %s \n",scope == NULL ? "GLOBAL" : scope);
    ht_table_t *hashmap;
    ht_init(&hashmap);
    if(hashmap == NULL) return false;

    listPushFirst(table->tables,hashmap);

    if(currentFunctionItem != NULL){
        table->currentFunction = currentFunctionItem;
    }

    if(scope != NULL){
        char *str = concatString(1,"empty_string");
        sprintf(str,"%d",table->gen->counter);
        char *string = concatString(2,scope,str);

        int stringLength = strlen(string) + 1;
        char *copied = (char *)malloc(stringLength);
        memcpy(copied,string,stringLength);

        listPushFirst(table->scopes,copied);
    }

    symtableItem *tempItem = table->activeItem;

    table->activeItem = tempItem;
    

    table->gen->counter++;

    return true;
}

void symtableExitScope(symtable *table){
    
    ht_table_t *hashmap;

    hashmap = (ht_table_t *)listGetFirst(table->tables);

    for (size_t i = 0; i < HT_SIZE; i++)
    {
        ht_item_t *currentItem = (*hashmap)[i];

        while(currentItem != NULL){
            symtableItem * item = (symtableItem *)(currentItem->data);
            if(item->funcData != NULL){
                listNode *argNode = item->funcData->arguments->first;
                while(argNode != NULL){
                    free(argNode->data);
                    argNode->data = NULL;
                    argNode = argNode->next;
                }
            }
            free(item->name);
            free(item->funcData);
            free(item);
            item->name = NULL;
            item->funcData = NULL;
            item = NULL;
            currentItem = currentItem->next;  
        }
    }
    ht_delete_all(hashmap);
    free(hashmap);
    
    listPopFirst(table->tables);
    
    if(listLength(table->scopes) != 0){
        char *scopeString = (char *)listGetFirst(table->scopes);
        
        if(strstr(scopeString, "&while") == NULL && strstr(scopeString, "&if") == NULL){
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

        if(strstr(scopeString, "&while") != NULL) {
            listNode *node = table->gen->temporary->first;
            while(node != NULL){
                if(table->currentFunction == NULL){
                    generatorPushStringToList(table->gen->mainCode,(char*)node->data);
                    free(node->data);
                    node->data = NULL;
                }else{
                    generatorPushStringToList(table->functionCodeBody,(char*)node->data);
                    free(node->data);
                    node->data = NULL;
                }
                node = node->next;
            }
            listClear(table->gen->temporary);
        }

        DEBUG_PRINTF("[Symtable] Exiting scope - %s \n",scopeString);
        free(scopeString);
        scopeString = NULL;
        listPopFirst(table->scopes);
    }else{
        DEBUG_PRINTF("[Symtable] Exiting scope - GLOBAL\n");
    }

}

void symtableFunctionDataFree(functionData *funcData){
    listNode *argNode = funcData->arguments->first;
    while(argNode != NULL){
        free(argNode->data);
        argNode->data = NULL;
        argNode = argNode->next;
    }

    listDestroy(funcData->arguments);
    free(funcData);
    funcData = NULL;
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
    if(symtableIsVariableDefined(table,varName)){
        DEBUG_PRINTF("[Symtable] Variable %s was already defined\n");
        raiseError(ERR_UNDEFINED_FUNCTION);
    }
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);

    int stringLength = strlen(varName) + 1;
    char *string = (char *)malloc(stringLength);
    memcpy(string,varName,stringLength);
    
    symtableItem *newSymtableItem = (symtableItem *)malloc(sizeof(symtableItem));
    newSymtableItem->name = string;
    newSymtableItem->funcData = NULL;
    newSymtableItem->type = DATA_TYPE_NOTSET;
    newSymtableItem->isConstant = false;
    newSymtableItem->valueIsSet = false;

    if(isFunction){
        newSymtableItem->funcData = (functionData *)malloc(sizeof(functionData));
        newSymtableItem->funcData->returnType = DATA_TYPE_NOTSET;
        newSymtableItem->funcData->arguments = listInit();
    }

    ht_insert(currentTable,string,newSymtableItem);

    table->activeItem = newSymtableItem;
}

void symtablePrintVariables(symtable *table){
    if(listLength(table->scopes) != 0){
        char *currentScope = (char *)listGetFirst(table->scopes);
        DEBUG_PRINTF("[Symtable] Variables on scope - %s \n",currentScope);
    }else{
        DEBUG_PRINTF("[Symtable] Variables on scope - GLOBAL \n");
    }
    
    ht_table_t *currentTable = (ht_table_t *)listGetFirst(table->tables);

    for (size_t i = 0; i < HT_SIZE; i++)
    {
        ht_item_t *currentItem = (*currentTable)[i];
        while(currentItem != NULL){
            symtableItem *item = (symtableItem *)(currentItem->data);
            
            if(item->funcData == NULL){
                DEBUG_PRINTF("[Symtable] %s (VAR, TYPE: %d) = ",item->name,item->type);
                if(item->type == DATA_TYPE_NOTSET) DEBUG_PRINTF("NOT SET\n");
            }else{
                DEBUG_PRINTF("[Symtable] %s (FUNC, RETURN TYPE: %d) - ",item->name,item->funcData->returnType);
                bool noArgs = false;
                functionArgument *argument = (functionArgument *)listPopFirst(item->funcData->arguments);
                if(argument == NULL){
                    DEBUG_PRINTF("NO ARGS\n");
                    noArgs = true;
                }
                while(argument != NULL){
                    DEBUG_PRINTF("%s (TYPE: %d) ",argument->name,argument->type);
                    argument = (functionArgument *)listPopFirst(item->funcData->arguments);
                }
                if(!noArgs){
                    DEBUG_PRINTF("\n");
                }
            }
            
            currentItem = currentItem->next;
        }
    }
}

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

void symtableVariableIsConstant(symtable *table){
    if(table == NULL) return;
    if(table->activeItem == NULL) return;

    table->activeItem->isConstant = true;
}

void symtableSetVariableValue(symtable *table){
    if(table == NULL) return;
    if(table->activeItem == NULL) return;
    
    if(table->activeItem->isConstant == true){
        if(table->activeItem->valueIsSet){
            if(table->activeItem->endOfDefinition == true){
                raiseError(ERR_SEMANTIC);
            }
        }
    }

    table->activeItem->valueIsSet = true;
    
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

void symtableFunctionEndOfArguments(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    table->activeItem->funcData->endOfArguments = true;

    symtableItem *oldActive = table->activeItem;

    symtableEnterScope(table,table->activeItem->name,table->activeItem);   

    listNode *node = table->activeItem->funcData->arguments->first;
    while(node != NULL){
        functionArgument *arg = (functionArgument *)node->data;
        symtableInsert(table,arg->id,false);
        symtableSetDataType(table,arg->type,arg->nullable);
        node = node->next;
    }

    table->activeItem = oldActive;

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
    DEBUG_PRINTF("---------------Is %s initieted? \n",varName);
    symtableItem *item = symtableFindSymtableItem(table,varName);
    if(item == NULL) return false;
    return item->valueIsSet;
}

bool symtableIsActiveVariableInitiated(symtable *table){
    DEBUG_PRINTF("---------------Is active initieted? \n");
    if(table == NULL) return false;
    if(table->activeItem == NULL) return false;
    return symtableIsVariableInitiated(table,table->activeItem->name);
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

bool symtableGetVariableNullable(symtable *table, char *varName){
    symtableItem *item = symtableFindSymtableItem(table,varName);

    if(item == NULL) raiseError(ERR_UNDEFINED_VARIABLE);
    if(item->funcData == NULL){
        return item->nullable;
    }else{
        return item->funcData->returnTypeNullable;
    }
}

void symtableCreateFunctionStructure(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    generatorPushStringToList(table->functionCodeHeader,"");
    generatorPushStringToList(table->functionCodeHeader,"#Start of function definition");
    generatorPushStringToList(table->functionCodeHeader,concatString(2,"LABEL $",table->activeItem->name));
    generatorPushStringToList(table->functionCodeHeader,"PUSHFRAME");
    generatorPushStringToList(table->functionCodeHeader,"DEFVAR LF@%retval");
    generatorPushStringToList(table->functionCodeHeader,"MOVE LF@%retval nil@nil");

    listNode *arg = table->activeItem->funcData->arguments->first;
    int i = 1;
    while(arg != NULL){
        //char str[128];
        
        functionArgument *argData = (functionArgument *)arg->data;
        char str2[128];
        //sprintf(str, "LF@param%d", i);

        char* str = concatString(2,symtableGetVariablePrefix(table,argData->id),argData->id);

        sprintf(str2, "LF@!%d",i);
        generatorPushStringToList(table->functionCodeHeader,concatString(2,"DEFVAR ",str));
        generatorPushStringToList(table->functionCodeHeader,concatString(4,"MOVE ",str," ",str2));
        i++;
        arg = arg->next;
    }

    generatorPushStringToList(table->functionCodeFooter,"POPFRAME");
    generatorPushStringToList(table->functionCodeFooter,"RETURN");
    generatorPushStringToList(table->functionCodeFooter,"#End of function definition");
}

void symtablePushCode(symtable *table, char* code){
    if(table == NULL) return;
    if(listLength(table->scopes) == 0){
        generatorPushStringToList(table->gen->mainCode,code);
    }else{
        //Check if we are in function
        if(table->currentFunction == NULL){
            char* scope = (char*)listGetFirst(table->scopes);
            if (strstr(scope, "&while") != NULL) {
                if (strstr(code, "DEFVAR ") != NULL) {
                    generatorPushStringFirstToList(table->gen->mainCode,code);
                }else{
                    generatorPushStringToList(table->gen->temporary,code);
                }
            }else{
                generatorPushStringToList(table->gen->mainCode,code);
            }
        }else{
            char* scope = (char*)listGetFirst(table->scopes);
            if (strstr(scope, "&while") != NULL) {
                if (strstr(code, "DEFVAR ") != NULL) {
                    generatorPushStringFirstToList(table->functionCodeBody,code);
                }else{
                    generatorPushStringToList(table->gen->temporary,code);
                }
            }else{
                generatorPushStringToList(table->functionCodeBody,code);
            }


            //generatorPushStringToList(table->functionCodeBody,code);
        }
    }
    
}

char* symtableGetScopePrefixName(symtable *table){
    if(listLength(table->scopes) == 0){
        return allocateString("global_");
    }else{
        return allocateString((char *)listGetFirst(table->scopes));
    }
}

char* symtableGetVariablePrefix(symtable *table, char *varName){
    if(listLength(table->scopes) == 0){
        return concatString(2,symtableGetFramePrefix(table,varName),symtableGetScopePrefixName(table));
    }else{
        listNode *currentNode = table->tables->first;
        listNode *scopeCurrentNode = table->scopes->first;
        while(currentNode != NULL){
            ht_table_t *currentTable = (ht_table_t *)(currentNode->data);
            ht_item_t *item = ht_search(currentTable,varName);
            if(item != NULL){
                if(scopeCurrentNode == NULL){
                    return concatString(2,symtableGetFramePrefix(table,varName),"global_");
                }else{
                    return concatString(3,symtableGetFramePrefix(table,varName),(char*)(scopeCurrentNode->data),"_");
                }
                
            }
            currentNode = currentNode->next;
            if(scopeCurrentNode != NULL){
                scopeCurrentNode = scopeCurrentNode->next;
            }
            
        }
    }
    return concatString(1,symtableGetFramePrefix(table,varName));
}

char* symtableGetFramePrefix(symtable *table, char *varName){
    ht_item_t *item = ht_search(((ht_table_t *)listGetLast(table->tables)),varName);
    if(item != NULL) return "GF@";

    if(listLength(table->scopes) != 0){
        if(table->currentFunction == NULL){
            return "GF@";
        }else{
            return "LF@";
        }
    }else{
        return "GF@";
    }
}

//FUNCTION CALLS ex. foo(par1, par2, par3);

void symtableFunctionCallStart(symtable *table, char *funcName){
    if(funcName != NULL) table->lastFunctionCall = funcName;
    DEBUG_PRINTF("[Symtable] Function call start CALL START %s \n",table->lastFunctionCall);
    //table->lastFunctionCall = funcName;
    
    functionData *funcData = (functionData *)(malloc(sizeof(functionData)));
    funcData->returnType = DATA_TYPE_NOTSET;
    funcData->arguments = listInit();
    funcData->callName = concatString(2,table->lastFunctionCall,"");

    listPushBack(table->functionCalls,funcData);
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

void symtableEndOfFile(symtable *table){
    while(listLength(table->functionCalls) != 0){
        functionData *funcData = (functionData *)listGetFirst(table->functionCalls);
        if(strcmp(funcData->callName,"write") == 0 
            || strcmp(funcData->callName,"readString") == 0 
            || strcmp(funcData->callName,"readInt") == 0
            || strcmp(funcData->callName,"readDouble") == 0
            || strcmp(funcData->callName,"Int2Double") == 0
            || strcmp(funcData->callName,"Double2Int") == 0
            || strcmp(funcData->callName,"length") == 0
            || strcmp(funcData->callName,"substring") == 0
            || strcmp(funcData->callName,"ord") == 0
            || strcmp(funcData->callName,"chr") == 0){

            symtableFunctionDataFree(funcData);
            funcData = (functionData *)listPopFirst(table->functionCalls);
            continue;
        }
        
        symtableItem *item = symtableFindSymtableItem(table,funcData->callName);
        if(item != NULL){
            if(item->funcData == NULL){
                DEBUG_PRINTF("[Symtable] In list function calls was normal variable :killemoji:\n");
                raiseError(ERR_UNDEFINED_FUNCTION);
            }

            listNode *callCurrentItem = funcData->arguments->first;
            listNode *funcCurrentItem = item->funcData->arguments->first;

            while(true){
                if(callCurrentItem == NULL && funcCurrentItem != NULL){
                    DEBUG_PRINTF("[Symtable] Call argument missing\n");
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                if(callCurrentItem != NULL && funcCurrentItem == NULL){
                    DEBUG_PRINTF("[Symtable] Func argument missing\n");
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                if(callCurrentItem == NULL && funcCurrentItem == NULL) break;
                functionArgument *callArg = (functionArgument *)(callCurrentItem->data);
                functionArgument *funcArg = (functionArgument *)(funcCurrentItem->data);

                if(strcmp(callArg->name,funcArg->name) != 0){
                    DEBUG_PRINTF("[Symtable] Call argument name doesn't match fuction argument name (%s != %s)\n",callArg->name,funcArg->name);
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                if(callArg->type != funcArg->type){
                    DEBUG_PRINTF("[Symtable] Call argument type doesn't match fuction argument type (%d != %d)\n",callArg->type,funcArg->type);
                    raiseError(ERR_WRONG_TYPE);
                }
                
                if(callArg->nullable != funcArg->nullable){
                    if(!(funcArg->nullable && !callArg->nullable)){
                        DEBUG_PRINTF("[Symtable] Call argument nullable doesn't match fuction argument nullable (%d != %d)\n",callArg->nullable,funcArg->nullable);
                        raiseError(ERR_WRONG_TYPE);
                    }
                }

                callCurrentItem = callCurrentItem->next;
                funcCurrentItem = funcCurrentItem->next;
            }

        }else{
            DEBUG_PRINTF("[Symtable] Function %s was not found \n",funcData->callName);
            raiseError(ERR_UNDEFINED_FUNCTION);
        }

        listNode *node = funcData->arguments->first;
        while(node != NULL){
            free(node->data);
            node->data = NULL;
            node = node->next;
        }

        listDestroy(funcData->arguments);
        free(funcData);
        funcData = (functionData *)listPopFirst(table->functionCalls);
    }

    DEBUG_PRINTF("[Symtable] File was ended\n");
}

void symtableSetActiveItem(symtable *table, char* varName){
    if(table == NULL) return;
    symtableItem *item = symtableFindSymtableItem(table,varName);
    if(item == NULL) return;
    table->activeItem = item;
}

void symtableSetFunctionCallName(symtable *table, char* funcName){
    if(table == NULL) return;
    //table->lastFunctionCall = "test";
    table->lastFunctionCall = concatString(2,funcName,"");
}

enum data_type symtableGetActiveItemType(symtable *table){
    if(table == NULL) return DATA_TYPE_NOTSET;
    if(table->activeItem == NULL) return DATA_TYPE_NOTSET;
    return table->activeItem->type;
}

void symtableCheckSameTypes(enum data_type type1,enum data_type type2){
    if(type1 == type2) {return;}
    raiseError(ERR_WRONG_TYPE);
}

void symtableSetEndOfVariableDefinition(symtable *table){
    if(table == NULL) return;
    if(table->activeItem == NULL) return;

    table->activeItem->endOfDefinition = true;
}