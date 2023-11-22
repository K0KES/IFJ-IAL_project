#include <stdio.h>
#include "main.h"
#include "parser.h"
#include "scanner.h"
#include "symtable.h"
#include "generator.h"

int main(int argc, char const *argv[]){
    //Program state init
    programState *programState = programStateInit(); 

    //Call parser
    parse(programState);

    //Generate output code
    generatorGenerateOutput(programState->gen);

    //symtablePrintVariables(symTable);

    programStateFree(programState);

    return 0;
}



programState *programStateInit(){
    programState *state = (programState*)malloc(sizeof(programState));
    if (state == NULL) {raiseError(ERR_INTERNAL);}

    state->tokenQueue = listInit();
    state->gen = generatorInit();
    state->symTable = symtableInit();
    state->activeToken = tokenInit();
    return state;
}

void programStateFree(programState *state){
    tokenFree(state->activeToken);
    symtableFree(state->symTable);
    generatorFree(state->gen);
    listDestroy(state->tokenQueue);

    free(state);
    state = NULL;
}

