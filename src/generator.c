#include "generator.h"
#include <stdio.h>
#include <stdarg.h>

generator* generatorInit(){
    generator *gen = (generator *)(malloc(sizeof(generator))); 
    if(gen == NULL) return NULL;

    gen->functions = listInit();
    gen->mainCode = listInit();
    gen->parserStack = listInit();
    gen->temporary = listInit();

    gen->counter = 0;

    return gen;
}

void generatorPushStringToList(list *list, char *string){
    int stringLength = strlen(string) + 1;
    char *text = (char *)malloc(stringLength);
    memcpy(text,string,stringLength);
    listPushFirst(list,text);
}

char* generatorPopStringFromList(list *list){
    return (char *)listPopFirst(list);
}

void generatorFree(generator *gen){
    if(gen == NULL) return;

    listDestroy(gen->functions);
    listDestroy(gen->mainCode);
    listDestroy(gen->parserStack);
    listDestroy(gen->temporary);

    free(gen);
    gen = NULL;
}

void generatorGenerateOutput(generator *gen){
    FILE *fptr;
    fptr = fopen("output", "w");
    fprintf(fptr, ".IFJcode23\n");
    fprintf(fptr, "JUMP $$main\n");

    //INSERT FUNCTIONS
    
    fprintf(fptr, "LABEL $$main\n");
    
    char* line = (char *)listPopLast(gen->mainCode);
    while(line != NULL){
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        line = (char *)listPopLast(gen->mainCode);
    }
    
    fclose(fptr);
}

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
}