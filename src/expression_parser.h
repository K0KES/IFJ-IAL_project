#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

    char precedenceTable[9][9] = {
        {'<', '=', '<', '<', '<', '<', '<', '<', ' '},
        {' ', '>', '>', '>', '>', '>', '>', ' ', '>'},
        {'<', '>', '>', '>', '>', '>', '>', '<', '>'},
        {'<', '>', '<', '>', '>', '>', '>', '<', '>'},
        {'<', '>', '<', '<', '>', '>', '>', '<', '>'},
        {'<', '>', '<', '<', '>', '>', '>', '<', '>'},
        {'<', '>', '<', '<', '>', '<', '<', '<', '>'},
        {' ', '>', '>', '>', '>', '>', '>', ' ', '>'},
        {'<', ' ', '<', '<', '<', '<', '<', '<', ' '}
    };



int parseExpression(token *tokenArr, unsigned tokenArrLenght, char thisWillBePointerOnTheTopOfTheSubDerivationTree);


/// @brief Function returns precedence of two tokens 
/// @param topOfStack token on the top of the stack
/// @param currentToken token that is being processed
/// @param precedenceTable source of precedence table
/// @return returns < > = or ' ' (space) based on precedence of two tokens
char getPrecedence(token topOfStack, token currentToken, char *precedenceTable);


/// 
/// @brief Get the Index In Precedence Table object
/// 
/// @param tokenType type of token whose index we want to get
/// @return unsigned int index of token in precedence table
/// 
unsigned int getIndexInPrecedenceTable(enum tokenType tokenType);


/*  Token stack*/

struct tokenStackElement
{
    token *tokenValue;
    struct tokenStackElement *next;
};

struct tokenStack
{
    struct tokenStackElement *top;
};

void tokenStackPush(struct tokenStack *stack, token *tokenValue)
{
    struct tokenStackElement *newElement = (struct tokenStackElement *) malloc(sizeof(struct tokenStackElement));
    newElement->tokenValue = tokenValue;
    newElement->next = stack->top;
    stack->top = newElement;
}

