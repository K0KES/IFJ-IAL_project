#include "error.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

list *allocatedStrings;

generator* generatorInit(){
    generator *gen = (generator *)(malloc(sizeof(generator))); 
    if(gen == NULL) return NULL;

    gen->functions = listInit();
    gen->mainCode = listInit();
    gen->parserStack = listInit();
    gen->temporary = listInit();
    allocatedStrings = listInit();

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

void generatorGenerateOutput(generator *gen){
    FILE *fptr = NULL;
    fptr = fopen("output", "w");
    fprintf(fptr, ".IFJcode23\n");
    fprintf(fptr, "JUMP $$main\n");

    char* line = (char *)listPopFirst(gen->functions);
    while(line != NULL){
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        free(line);
        line = (char *)listPopFirst(gen->functions);
    }
    
    fprintf(fptr, "LABEL $$main\n");
    
    line = (char *)listPopFirst(gen->mainCode);
    while(line != NULL){
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        free(line);
        line = (char *)listPopFirst(gen->mainCode);
    }
    
    fclose(fptr);
}

void generatorGenerateOutputToStdOut(generator *gen){
    printf(".IFJcode23\n");
    printf("JUMP $$main\n");

    char* line = (char *)listPopFirst(gen->functions);
    while(line != NULL){
        printf("%s\n",line);
        free(line);
        line = (char *)listPopFirst(gen->functions);
    }
    
    printf("LABEL $$main\n");
    
    line = (char *)listPopFirst(gen->mainCode);
    while(line != NULL){
        printf("%s\n",line);
        free(line);
        line = (char *)listPopFirst(gen->mainCode);
    }
}

/*
char * concatString(int num_args, ...){
    va_list args;
    va_start(args, num_args);

    char* first = va_arg(args, char *);
    int stringLength = strlen(first) + 1;
    char *newFirst = (char *)malloc(stringLength);
    memcpy(newFirst,first,stringLength);

    for(int i = 1; i < num_args; i++) {
        char* text = va_arg(args, char *);
        strcat(newFirst,text);
    }

    va_end(args);
    return newFirst;
}*/

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
    static char result[100];
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
        free(node->data);
        node = node->next;
    }
    listDestroy(list);
    list = NULL;
}