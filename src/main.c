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

    //Symtable init
    symtable *symTable = symtableInit();

    //Generator init
    generator *gen = generatorInit();

    //Call parser
    parse(activeToken,symTable,gen);

    generatorGenerateOutput(gen);

    //symtablePrintVariables(symTable);
    //generatorFree(gen);
    symtableFree(symTable);
    tokenFree(activeToken);

    return 0;
}
