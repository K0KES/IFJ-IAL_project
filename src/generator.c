#include "error.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "symtable.h"

list *allocatedStrings;

generator* generatorInit(){
    generator *gen = (generator *)(malloc(sizeof(generator))); 
    if(gen == NULL) return NULL;

    gen->functions = listInit();
    gen->mainCode = listInit();
    gen->parserStack = listInit();
    gen->temporary = listInit();
    allocatedStrings = listInit();
    ht_init(&gen->functionCallsTable);

    gen->counter = 1;

    return gen;
}

void generatorPushStringFirstToList(list *list, char *string){
    int stringLength = strlen(string) + 1;
    char *text = (char *)malloc(stringLength);
    memcpy(text,string,stringLength);
    listPushFirst(list,text);
}

void generatorPushStringToList(list *list, char *string){
    int stringLength = strlen(string) + 1;
    char *text = (char *)malloc(stringLength);
    memcpy(text,string,stringLength);
    listPushBack(list,text);
}

char* generatorPopFirstStringFromList(list *list){
    char* line = (char *)listPopFirst(list);
    listPushBack(allocatedStrings,line);
    return line;
}

char* generatorPopLastStringFromList(list *list){
    return (char *)listPopLast(list);
}

char* generatorGetFirstStringFromList(list *list){
    return (char *)listGetFirst(list);
}

char* generatorGetLastStringFromList(list *list){
    return (char *)listGetLast(list);
}

void generatorFree(generator *gen){
    if(gen == NULL) return;

    freeContentOfListAndDestroy(gen->functions);
    freeContentOfListAndDestroy(gen->mainCode);
    freeContentOfListAndDestroy(gen->parserStack);
    freeContentOfListAndDestroy(gen->temporary);

    if(allocatedStrings != NULL){
        freeContentOfListAndDestroy(allocatedStrings);
    }

    free(gen);
    gen = NULL;
}

char* generatorProccessFunctionCall(generator *gen, char* line){
    symtable *table = (symtable *)gen->table;
    ht_item_t *item = ht_search(gen->functionCallsTable,line);
    if(item != NULL){
        functionData * callData = (functionData *)item->data;
        symtableItem *owner = symtableFindSymtableItem(table,callData->callName);

        if(listLength(owner->funcData->overloadFunctions) == 0){
            return concatString(2,"CALL $",callData->callName);
        }else{
            listNode *overloadNode = owner->funcData->overloadFunctions->first;
            int i = 1;

            if(symtableCheckIfOverloadMatches(callData,owner->funcData)){
                return concatString(2,"CALL $",callData->callName);
            }

            while(overloadNode != NULL){
                if(symtableCheckIfOverloadMatches(callData,(functionData *)overloadNode->data)){
                    char *str = allocateString("empty_string");
                    sprintf(str,"%d",i);
        
                    return concatString(3,"CALL $",callData->callName,str);
                }
                i++;
                overloadNode = overloadNode->next;
            }
        }
    }else{
        return line;
    }
}

void generatorGenerateOutput(generator *gen){
    FILE *fptr = NULL;
    fptr = fopen("output", "w");
    fprintf(fptr, ".IFJcode23\n");
    fprintf(fptr, "JUMP $$main\n");

    char* line = (char *)listPopFirst(gen->functions);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->functions);
    }
    
    fprintf(fptr, "\nLABEL $$main\n");
    
    line = (char *)listPopFirst(gen->mainCode);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->mainCode);
    }
    
    fclose(fptr);
    fptr = NULL;
}


void generatorGenerateOutputToStdOut(generator *gen){
    printf(".IFJcode23\n");
    printf("JUMP $$main\n");

    char* line = (char *)listPopFirst(gen->functions);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        printf("%s\n",line);
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->functions);
    }
    
    printf("\nLABEL $$main\n");
    
    line = (char *)listPopFirst(gen->mainCode);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        printf("%s\n",line);
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->mainCode);
    }
}

char * allocateString(char * original){
    char * copy = malloc(strlen(original) + 18); //Why not
    strcpy(copy, original);
    
    listPushBack(allocatedStrings,copy);
    return copy;
}

char * concatString(int num_args, ...){
    va_list args;
    va_start(args, num_args);

    int length = 0;
    for (int i = 0; i < num_args; ++i){
        char* text = va_arg(args, char *);
        length += strlen(text);
    }
    
    char *output = (char*)malloc(length + 1);

    va_start(args, num_args);

    char *dest = output;
    for (int i = 0; i < num_args; ++i) {
        char *src = va_arg(args, char *);
        while (*src)
            *dest++ = *src++;
    }
    *dest = '\0';

    va_end(args);
    
    listPushBack(allocatedStrings,output);
    return output;
}

char* generatorGenerateTempVarName(generator *gen){
    char* result = allocateString("musim_alokovat_takhle_xd");
    snprintf(result, sizeof(result), "temp%d", gen->counter);
    gen->counter++;
    return result;
}

char* stringToAssemblyStringFormat(const char* inputString) {
    int length = 0;
    const char* p = inputString;

    // Spočítáme délku výsledného řetězce
    while (*p) {
        // Escape sekvence nebo bílý znak
        if (*p == '\\' || *p == '#' || (*p >= 0 && *p <= 32)) { length += 5;} //Escape sekvence má 5 znaků (\x00)
        else {length++;}
        p++;
    }

    char* outputString = (char*)malloc(length + 1);
    if (!outputString) { raiseError(ERR_INTERNAL); }

    char* q = outputString;

    // Kopírujeme znaky a vytváříme escape sekvence
    p = inputString;
    while (*p) {
        if (*p == '\\' || *p == '#' || (*p >= 0 && *p <= 32)) {
            // Escape sekvence
            q += sprintf(q, "\\%03d", *p);
        } else {
            // Běžný tisknutelný znak
            *q++ = *p;
        }
        p++;
    }

    // Nastavíme koncový znak
    *q = '\0';
    return outputString;
}

void printList(list *l){
    listNode *currentNode = l->first;
    DEBUG_PRINTF("[Generator] LIST PRINT: ");
    while(currentNode != NULL){
        char* line = (char *)(currentNode->data);
        DEBUG_PRINTF("%s ",line);
        currentNode = currentNode->next;
    }
    DEBUG_PRINTF("\n");
}

void freeContentOfListAndDestroy(list *list){
    listNode *node = list->first;

    while(node != NULL){
        if(node->data != NULL){
            free(node->data);   
            node->data = NULL;
        }
        node = node->next;
    }
    
    listDestroy(list);
    list = NULL;
}