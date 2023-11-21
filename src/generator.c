#include "generator.h"
#include <stdio.h>

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
    
    fprintf(fptr, "LABEL $$main");
    //INSERT MAIN CODE
    
    fclose(fptr);

    
}