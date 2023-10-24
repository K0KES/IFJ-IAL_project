#include <stdio.h>
#include "main.h"
#include "parser.h"
#include "scanner.h"

int main(int argc, char const *argv[]){
    token *activeToken = malloc(sizeof(token));
    if(activeToken == NULL){
        return 1;
    }

    activeToken->value = (string*)malloc(sizeof(string));
    if(activeToken->value == NULL){
        return 1;
    }
    strInit(activeToken->value);

    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    if(activeToken->position == NULL){
        return 1;
    }
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    parse(activeToken);
    return 0;
}
