/**
 * @file symtable.c
 * @author Vít Slavíček
 * @brief Symtable
 * @date 9.10.2023
*/

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

    table->createFrameCounter = 0;
    table->inExpression = false;

    generator->table = (void *)table;

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
        char *str = allocateString("empty_string");
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
        functionArgument *arg = (functionArgument *)argNode->data;


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
    //listDestroy(table->functionCalls);

    freeContentOfListAndDestroy(table->functionCodeFooter);
    freeContentOfListAndDestroy(table->functionCodeBody);
    freeContentOfListAndDestroy(table->functionCodeHeader);
    free(table);
}

void symtableInsert(symtable *table, char *varName, bool isFunction){
    if(symtableIsVariableDefinedInCurrentScope(table,varName)){
        if(!isFunction){
            DEBUG_PRINTF("[Symtable] Variable %s was already defined\n");
            raiseError(ERR_UNDEFINED_FUNCTION);
        }else{
            symtableItem *owner = symtableFindSymtableItem(table,varName);

            functionData *funcData = (functionData *)malloc(sizeof(functionData));
            funcData->returnType = DATA_TYPE_VOID;
            funcData->arguments = listInit();

            listPushBack(owner->funcData->overloadFunctions,funcData);
            table->activeItem = owner;
            return;
        }
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
        newSymtableItem->funcData->returnType = DATA_TYPE_VOID;
        newSymtableItem->funcData->arguments = listInit();
        newSymtableItem->funcData->overloadFunctions = listInit();
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
        functionData *currentFunction = NULL;
        if(listLength(table->activeItem->funcData->overloadFunctions) == 0){
            currentFunction = table->activeItem->funcData;
        }else{
            currentFunction = (functionData *)listGetLast(table->activeItem->funcData->overloadFunctions);
        }
        
        if(currentFunction->endOfArguments == false){
            functionArgument *argument = (functionArgument *)listGetLast(currentFunction->arguments);
            argument->type = type;
            argument->nullable = nullable;
            
        }else{
            currentFunction->returnType = type;
            currentFunction->returnTypeNullable = nullable;
            
        }
    }
}

void symtableVariableIsConstant(symtable *table){
    if(table == NULL) return;
    if(table->activeItem == NULL) return;

    table->activeItem->isConstant = true;
}

void symtableVariableIsNotConstant(symtable *table){
    if(table == NULL) return;
    if(table->activeItem == NULL) return;

    table->activeItem->isConstant = false;
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

    functionData *currentFunction = NULL;
    if(listLength(table->activeItem->funcData->overloadFunctions) == 0){
        currentFunction = table->activeItem->funcData;
    }else{
        currentFunction = (functionData *)listGetLast(table->activeItem->funcData->overloadFunctions);
    }

    listPushBack(currentFunction->arguments,argument);
}

void symtableSetFunctionArgumentID(symtable *table, char *id){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    char* string = allocateString(id);

    functionData *currentFunction = NULL;
    if(listLength(table->activeItem->funcData->overloadFunctions) == 0){
        currentFunction = table->activeItem->funcData;
    }else{
        currentFunction = (functionData *)listGetLast(table->activeItem->funcData->overloadFunctions);
    }

    functionArgument *argument = (functionArgument *)listGetLast(currentFunction->arguments);
    if(strcmp(argument->name,string) == 0){
        DEBUG_PRINTF("[Symtable] Function argument name and ID was identical\n");
        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
    }
    argument->id = string;
}

void symtableSetFunctionArgumentName(symtable *table, char *name){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    char* string = allocateString(name);

    functionData *currentFunction = NULL;
    if(listLength(table->activeItem->funcData->overloadFunctions) == 0){
        currentFunction = table->activeItem->funcData;
    }else{
        currentFunction = (functionData *)listGetLast(table->activeItem->funcData->overloadFunctions);
    }

    functionArgument *argument = (functionArgument *)listGetLast(currentFunction->arguments);
    argument->name = string;
}

bool symtableCheckIfOverloadMatches(functionData *callData, functionData *funcData){
    listNode *callNode = callData->arguments->first;
    listNode *funcNode = funcData->arguments->first;
    while(true){
        if(callNode == NULL && funcNode != NULL){
            return false;
        }
        if(callNode != NULL && funcNode == NULL){
            return false;
        }
        if(callNode == NULL && funcNode == NULL) break;
        functionArgument *callArg = (functionArgument *)(callNode->data);
        functionArgument *funcArg = (functionArgument *)(funcNode->data);

        if(strcmp(funcArg->name,callArg->name) != 0){
            return false;
        } 
        if(funcArg->type != callArg->type){
            return false;
        }
        if(funcArg->nullable != callArg->nullable){
            return false;
        }

        callNode = callNode->next;
        funcNode = funcNode->next;
    }

    DEBUG_PRINTF("CALL %d FUNC %d \n",callData->returnType,funcData->returnType);
    if(callData->returnType != funcData->returnType){
        return false;
    }

    return true;
}

void symtableFunctionEndOfArguments(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    symtableItem *owner = symtableFindSymtableItem(table,table->activeItem->name);

    functionData *currentFunction = NULL;
    if(listLength(owner->funcData->overloadFunctions) == 0){
        currentFunction = owner->funcData;
    }else{
        //TODOOOOO
        currentFunction = (functionData *)listGetLast(owner->funcData->overloadFunctions);
        
        DEBUG_PRINTF("ARGS: %d ARGS: %d \n",listLength(currentFunction->arguments),listLength(owner->funcData->arguments));

        if(symtableCheckIfOverloadMatches(currentFunction,owner->funcData)){
            DEBUG_PRINTF("[Symtable] Overload functions are same 2\n");
            raiseError(ERR_UNDEFINED_FUNCTION);
        }

        listNode *overloadNode = owner->funcData->overloadFunctions->first;
        while(overloadNode->next != NULL){
            if(symtableCheckIfOverloadMatches(currentFunction,(functionData *)(overloadNode->data))){
                DEBUG_PRINTF("[Symtable] Overload functions are same\n");
                raiseError(ERR_UNDEFINED_FUNCTION);
            }
            overloadNode = overloadNode->next;
        }
        
    }


    currentFunction->endOfArguments = true;

    symtableItem *oldActive = table->activeItem;

    symtableEnterScope(table,table->activeItem->name,table->activeItem);   

    /*
    listNode *node = currentFunction->arguments->first;
    while(node != NULL){
        functionArgument *arg = (functionArgument *)node->data;
        symtableInsert(table,arg->id,false);
        symtableVariableIsConstant(table);
        symtableSetDataType(table,arg->type,arg->nullable);
        symtableSetVariableValue(table);
        symtableSetEndOfVariableDefinition(table);
        
        node = node->next;
    }*/

    table->activeItem = oldActive;

    symtableCreateFunctionStructure(table);
}

enum data_type symtableGetReturnTypeOfCurrentScope(symtable *table){
    if(table->currentFunction == NULL) return DATA_TYPE_NOTSET;
    return table->currentFunction->funcData->returnType;
}

symtableItem *symtableFindSymtableItemInCurrentScope(symtable *table, char *varName){
    ht_table_t *currentTable = (ht_table_t *)(table->tables->first->data);
    ht_item_t *item = ht_search(currentTable,varName);
    if(item != NULL){
        return (symtableItem *)(item->data);
    }
    return NULL;
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

bool symtableIsVariableDefinedInCurrentScope(symtable *table,char *varName){
    return symtableFindSymtableItemInCurrentScope(table,varName) != NULL;
}

bool symtableIsVariableDefined(symtable *table,char *varName){
    if(table->currentFunction != NULL){
        functionData *overload = symtableGetOverloadedFunction(table,table->currentFunction->name);
        listNode *argNode = overload->arguments->first;
        while(argNode != NULL){
            functionArgument* arg = (functionArgument *)(argNode->data);
            if(strcmp(arg->id,varName) == 0){
                return true;
            }
            argNode = argNode->next;
        }
    }

    return symtableFindSymtableItem(table,varName) != NULL;
}

bool symtableIsVariableInitiated(symtable *table,char *varName){
    DEBUG_PRINTF("---------------Is %s initieted? \n",varName);
    symtableItem *item = symtableFindSymtableItem(table,varName);

    if(table->currentFunction != NULL){
        functionData *overload = symtableGetOverloadedFunction(table,table->currentFunction->name);
        listNode *argNode = overload->arguments->first;
        while(argNode != NULL){
            functionArgument* arg = (functionArgument *)(argNode->data);
            if(strcmp(arg->id,varName) == 0){
                return true;
            }
            argNode = argNode->next;
        }
    }

    if(item == NULL) return false;
    return item->valueIsSet;
}

bool symtableIsActiveVariableInitiated(symtable *table){
    DEBUG_PRINTF("---------------Is active initieted? \n");
    if(table == NULL) return false;
    if(table->activeItem == NULL) return false;
    return symtableIsVariableInitiated(table,table->activeItem->name);
}

functionData * symtableGetOverloadedFunction(symtable *table, char* funcName){
    symtableItem *owner = symtableFindSymtableItem(table,funcName);

    functionData *currentFunction = NULL;
    if(listLength(owner->funcData->overloadFunctions) == 0){
        currentFunction = owner->funcData;
    }else{
        currentFunction = (functionData *)listGetLast(owner->funcData->overloadFunctions);
    }

    return currentFunction;
}

enum data_type symtableGetVariableType(symtable *table, char *varName){
    symtableItem *item = symtableFindSymtableItem(table,varName);

    if(table->currentFunction != NULL){
        functionData *overload = symtableGetOverloadedFunction(table,table->currentFunction->name);
        listNode *argNode = overload->arguments->first;
        while(argNode != NULL){
            functionArgument* arg = (functionArgument *)(argNode->data);
            if(strcmp(arg->id,varName) == 0){
                return arg->type;
            }
            argNode = argNode->next;
        }
    }

    if(item == NULL) return DATA_TYPE_NOTSET;
    if(item->funcData == NULL){
        return item->type;
    }else{
        return item->funcData->returnType;
    }
}

bool symtableGetVariableNullable(symtable *table, char *varName){
    symtableItem *item = symtableFindSymtableItem(table,varName);

    if(table->currentFunction != NULL){
        functionData *overload = symtableGetOverloadedFunction(table,table->currentFunction->name);
        listNode *argNode = overload->arguments->first;
        while(argNode != NULL){
            functionArgument* arg = (functionArgument *)(argNode->data);
            if(strcmp(arg->id,varName) == 0){
                return arg->nullable;
            }
            argNode = argNode->next;
        }
    }

    if(item == NULL) return false;
    if(item->funcData == NULL){
        return item->nullable;
    }else{
        return item->funcData->returnTypeNullable;
    }
}

void symtableSetVariableNullable(symtable *table, char *varName, bool nullable){
    symtableItem *item = symtableFindSymtableItem(table,varName);

    if(item == NULL) return;
    if(item->funcData == NULL){
        item->nullable = nullable;
        return;
    }else{
        item->funcData->returnTypeNullable = nullable;
        return;
    }
}

void symtableCreateFunctionStructure(symtable *table){
    if(table->activeItem == NULL) return;
    if(table->activeItem->funcData == NULL) return;

    functionData *currentFunction = NULL;
    char *activeFunctionName = NULL;
    if(listLength(table->activeItem->funcData->overloadFunctions) == 0){
        currentFunction = table->activeItem->funcData;
        activeFunctionName = table->activeItem->name;
    }else{
        currentFunction = (functionData *)listGetLast(table->activeItem->funcData->overloadFunctions);
        
        char *str = allocateString("empty_string");
        sprintf(str,"%d",listLength(table->activeItem->funcData->overloadFunctions));
        
        activeFunctionName = concatString(2,table->activeItem->name,str);
    }

    

    generatorPushStringToList(table->functionCodeHeader,"");
    generatorPushStringToList(table->functionCodeHeader,"#Start of function definition");
    generatorPushStringToList(table->functionCodeHeader,concatString(2,"LABEL $",activeFunctionName));
    generatorPushStringToList(table->functionCodeHeader,"PUSHFRAME");
    generatorPushStringToList(table->functionCodeHeader,"DEFVAR LF@%retval");
    generatorPushStringToList(table->functionCodeHeader,"MOVE LF@%retval nil@nil");

    listNode *arg = currentFunction->arguments->first;
    
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

    generatorPushStringToList(table->functionCodeFooter,concatString(3,"LABEL $",activeFunctionName,"_end"));
    generatorPushStringToList(table->functionCodeFooter,"POPFRAME");
    generatorPushStringToList(table->functionCodeFooter,"RETURN");
    generatorPushStringToList(table->functionCodeFooter,"#End of function definition");
}

void symtablePushCodeCreateFrame(symtable *table){
    table->createFrameCounter ++;
    if(table->createFrameCounter != 1){
        return;
    }
    symtablePushCode(table,"CREATEFRAME");
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
    if(table->currentFunction != NULL){
        if(table->activeItem->valueIsSet == false){
            if(table->inExpression){
                functionData *overload = symtableGetOverloadedFunction(table,table->currentFunction->name);
                listNode *argNode = overload->arguments->first;
                while(argNode != NULL){
                    functionArgument* arg = (functionArgument *)(argNode->data);
                    if(strcmp(arg->id,varName) == 0){
                        return allocateString("LF@%");
                    }
                    argNode = argNode->next;
                }
            }
        }
    }
    
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
    if(strstr(varName,"temp") != NULL){
        return allocateString(symtableGetFramePrefix(table,varName));
    }else{
        return allocateString("LF@%");
    }

    
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
    funcData->returnType = DATA_TYPE_VOID;
    funcData->arguments = listInit();
    funcData->callName = concatString(2,table->lastFunctionCall,"");

    table->lastFunctionCall = concatString(3,"$$$",generatorGenerateTempVarName(table->gen),"$$$");
    ht_insert(table->gen->functionCallsTable,table->lastFunctionCall,funcData);

    listPushBack(table->functionCalls,funcData);
}

int symtableFunctionCallGetNumberOfParameters(symtable *table){
    functionData *funcData = (functionData *)listGetLast(table->functionCalls);
    return listLength(funcData->arguments);
}

void symtableFunctionCallEnd(symtable *table){
    DEBUG_PRINTF("[Symtable] Function call ENDED \n");
    
    if(listLength(table->functionCalls) > 1){
        functionData *funcData = (functionData *)listPopLast(table->functionCalls);
        listPushFirst(table->functionCalls,funcData);
    }
    if(table->createFrameCounter > 0){
        table->createFrameCounter--;
    }
    if(table->createFrameCounter != 0){
        symtablePushCode(table,"CREATEFRAME");
    }
}

void symtableFunctionCallNextParameter(symtable *table){
    DEBUG_PRINTF("[Symtable] Function call next parameter\n");
    functionData *funcData = (functionData *)listGetLast(table->functionCalls);
    
    functionArgument *argument = (functionArgument *)malloc(sizeof(functionArgument));
    argument->type = DATA_TYPE_NOTSET;
    argument->name = "_";
    argument->id = "";
    argument->nullable = false;
    listPushBack(funcData->arguments,argument);
}

void symtableFunctionCallSetParameterType(symtable *table, enum data_type type, bool nullable){
    DEBUG_PRINTF("[Symtable] Function call parameter type set\n");
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

void symtableCheckOverload(symtable *table,functionData *funcCall){
    
    symtableItem *owner = symtableFindSymtableItem(table,funcCall->callName);
    DEBUG_PRINTF("PROCCESSING OVERLOAD %s call args: %d \n",owner->name,listLength(funcCall->arguments));
    
    
    if(symtableCheckIfOverloadMatches(funcCall,owner->funcData)){
        DEBUG_PRINTF("FOUND OVERLOAD :) \n");
        return;
    }
    
    listNode *overloadNode = owner->funcData->overloadFunctions->first;
    int overloadId = 1;
    while(overloadNode != NULL){
        functionData *overloadFunc = (functionData *)overloadNode->data;
        DEBUG_PRINTF("COmparing overload... args: %d \n",listLength(overloadFunc->arguments));
        

        if(symtableCheckIfOverloadMatches(funcCall,overloadFunc)){
            DEBUG_PRINTF("FOUND OVERLOAD :) \n");
            return;
        }

        overloadId++;
        overloadNode = overloadNode->next;
    }

    DEBUG_PRINTF("[Symtable] Overload not found \n");
    raiseError(ERR_UNDEFINED_FUNCTION);


    //symtableFunctionDataFree(funcCall);
}

void symtableEndOfFile(symtable *table){
    listNode *funcDataNode = table->functionCalls->first;

    while(funcDataNode != NULL){
        functionData *funcData = (functionData *)funcDataNode->data;
        DEBUG_PRINTF("FUNC: %s callargs:%d\n",funcData->callName,listLength(funcData->arguments));

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

            //symtableFunctionDataFree(funcData);
            //funcData = (functionData *)listPopFirst(table->functionCalls);
            funcDataNode = funcDataNode->next;
            continue;
        }

        symtableItem *item = symtableFindSymtableItem(table,funcData->callName);
        if(item != NULL){
            if(listLength(item->funcData->overloadFunctions) != 0){
                symtableCheckOverload(table,funcData);
                //funcData = (functionData *)listPopFirst(table->functionCalls);
                funcDataNode = funcDataNode->next;
                continue;
            }
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
                    DEBUG_PRINTF("[Symtable] Call argument type doesn't match fuction argument type (%s(%d) != %s(%d))\n",callArg->name,callArg->type,funcArg->name,funcArg->type);
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                
                if(callArg->nullable != funcArg->nullable){
                    if(!(funcArg->nullable && !callArg->nullable)){
                        DEBUG_PRINTF("[Symtable] Call argument nullable doesn't match fuction argument nullable (%d != %d)\n",callArg->nullable,funcArg->nullable);
                        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                    }
                }

                callCurrentItem = callCurrentItem->next;
                funcCurrentItem = funcCurrentItem->next;
            }

        }else{
            DEBUG_PRINTF("[Symtable] Function %s was not found \n",funcData->callName);
            raiseError(ERR_UNDEFINED_FUNCTION);
        }

        //symtableFunctionDataFree(funcData);
        //funcData = (functionData *)listPopFirst(table->functionCalls);
        funcDataNode = funcDataNode->next;
    }

    DEBUG_PRINTF("[Symtable] File was ended\n");
}

void symtableSetActiveItem(symtable *table, char* varName){
    if(table == NULL) return;
    symtableItem *item = symtableFindSymtableItem(table,varName);
    if(item == NULL){
        if(table->currentFunction != NULL){
            functionData *overload = symtableGetOverloadedFunction(table,table->currentFunction->name);
            listNode *argNode = overload->arguments->first;
            while(argNode != NULL){
                functionArgument* arg = (functionArgument *)(argNode->data);
                if(strcmp(arg->id,varName) == 0){
                    DEBUG_PRINTF("[Symtable] Expected manipulation with function argument\n");
                    raiseError(ERR_SEMANTIC);
                }
                argNode = argNode->next;
            }
        }
    }
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

char *symtableGetActiveItemName(symtable *table){
    if(table == NULL) return DATA_TYPE_NOTSET;
    if(table->activeItem == NULL) return DATA_TYPE_NOTSET;
    return table->activeItem->name;
}

void symtableCheckSameTypes(enum data_type type1,enum data_type type2){
    if(type1 == type2) {return;}
    DEBUG_PRINTF("[Symtable] types doesn't match\n");
    raiseError(ERR_WRONG_TYPE);
}

void symtableSetEndOfVariableDefinition(symtable *table){
    if(table == NULL) return;
    if(table->activeItem == NULL) return;

    table->activeItem->endOfDefinition = true;
}