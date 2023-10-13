#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>

// Lenght of array of accepted token types
unsigned int acceptedTokenTypesLenght = 14;

// Array of accepted token types
enum tokenType acceptedTokenTypes[] = {
    T_IDENTIFIER,
    T_LEFT_BRACKET,
    T_RIGHT_BRACKET,    
    T_PLUS,
    T_MINUS,
    T_MULTIPLICATION,
    T_DIVISION,
    T_LESS,
    T_LESS_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_EQUAL,
    T_NOT_EQUAL,
    T_NIL_OP
};

int parseExpression(token *tokenArr, unsigned tokenArrLenght, char thisWillBePointerOnTheTopOfTheSubDerivationTree);

