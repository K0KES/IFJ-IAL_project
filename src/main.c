#include <stdio.h>
#include "main.h"
#include "parser.h"
#include "scanner.h"
#include "symtable.h"
#include "generator.h"

int main(int argc, char const *argv[]){
    //Parser init
    token *activeToken;
    //tokenInit(activeToken);
    activeToken = malloc(sizeof(token));
    if(activeToken == NULL){
        return;
    }

    activeToken->value = (string*)malloc(sizeof(string));
    if(activeToken->value == NULL){
        return;
    }
    strInit(activeToken->value);

    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    if(activeToken->position == NULL){
        return;
    }
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    //Program state init
    programState *programState = programStateInit(); 

    //Call parser
    parse(activeToken,programState);

    generatorGenerateOutput(programState->gen);

    //symtablePrintVariables(symTable);

    programStateFree(programState);
    tokenFree(activeToken);

    return 0;
}



programState *programStateInit(){
    programState *state = (programState*)malloc(sizeof(programState));
    if (state == NULL) {raiseError(ERR_INTERNAL);}

    state->tokenQueue = listInit();
    state->gen = generatorInit();
    state->symTable = symtableInit(state->gen);
    return state;
}

void programStateFree(programState *state){
    generatorFree(state->gen);
    symtableFree(state->symTable);
    listDestroy(state->tokenQueue);

    free(state);
    state = NULL;
}

