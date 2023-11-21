#include <stdio.h>
#include "main.h"
#include "parser.h"
#include "scanner.h"
#include "symtable.h"

//#define printf(...) fprintf(fptr, __VA_ARGS__)

int main(int argc, char const *argv[]){

    //FILE *fptr;
    //fptr = fopen("output","w");

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

    parse(activeToken);
    return 0;
}
