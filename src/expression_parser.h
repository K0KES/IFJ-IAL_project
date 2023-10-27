#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#pragma once


// Length of array of accepted token types
unsigned int acceptedTokenTypesLength = 18;

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
    T_NIL_OP,
    T_EOL,
    T_INT,
    T_DOUBLE,
    T_STRING
    };

char precedenceTable[9][9] = {
    {'<', '=', '<', '<', '<', '<', '<', '<', '1'},
    {'1', '>', '>', '>', '>', '>', '>', '1', '>'},
    {'<', '>', '>', '>', '>', '>', '>', '<', '>'},
    {'<', '>', '<', '>', '>', '>', '>', '<', '>'},
    {'<', '>', '<', '<', '>', '>', '>', '<', '>'},
    {'<', '>', '<', '<', '>', '>', '>', '<', '>'},
    {'<', '>', '<', '<', '>', '<', '<', '<', '>'},
    {'1', '>', '>', '>', '>', '>', '>', '1', '>'},
    {'<', '1', '<', '<', '<', '<', '<', '<', '0'}};



/// @brief Copy values for t1 to t2
/// @param t1 Pointer to source token
/// @param t2 Pointer to destination token
void copy_Token (token *t1, token *t2){
    t2->lastChar = t1->lastChar;
    t2->position = t1->position;
    t2->tokenType = t1->tokenType;
    t2->value = t2->value;
}
/* Precedence rules */

/// @brief Structure that stores one precedence rule
struct precedenceRule
{
    // Basic description that is meant to be used only for debugging
    char *description;
    // Length of token list
    unsigned rightSideLen;
    // List of tokens that are meant to be reduced
    token *rightSide;
    // Token to which they should be reduced
    token leftSide;
};

/// @brief Structure that stores all precedence rules
struct precedenceRulesList
{
    // Length of precedence rule list
    unsigned precedenceRuleListLen;
    // List of all precedence rules
    struct precedenceRule *precedenceRuleList;
};

int getPrecedenceRulesList(struct precedenceRulesList *outputPrecedenceRulesList)
{
    outputPrecedenceRulesList->precedenceRuleListLen = 13;
    outputPrecedenceRulesList->precedenceRuleList = ( struct precedenceRule *) malloc(sizeof(struct precedenceRule) * outputPrecedenceRulesList->precedenceRuleListLen);
    if (outputPrecedenceRulesList->precedenceRuleList == NULL)
    {
        return 99;
    }

    outputPrecedenceRulesList->precedenceRuleList[0].description = "E -> (E)";
    outputPrecedenceRulesList->precedenceRuleList[0].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[0].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[0].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[0].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[0].rightSide[0].tokenType = T_LEFT_BRACKET;
    outputPrecedenceRulesList->precedenceRuleList[0].rightSide[1].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[0].rightSide[2].tokenType = T_RIGHT_BRACKET;
    outputPrecedenceRulesList->precedenceRuleList[0].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[1].description = "E -> E * E";
    outputPrecedenceRulesList->precedenceRuleList[1].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[1].rightSide = (token *)malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[1].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[1].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[1].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[1].rightSide[1].tokenType = T_MULTIPLICATION;
    outputPrecedenceRulesList->precedenceRuleList[1].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[1].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[2].description = "E -> E / E";
    outputPrecedenceRulesList->precedenceRuleList[2].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[2].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[2].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[2].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[2].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[2].rightSide[1].tokenType = T_DIVISION;
    outputPrecedenceRulesList->precedenceRuleList[2].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[2].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[3].description = "E -> E + E";
    outputPrecedenceRulesList->precedenceRuleList[3].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[3].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[3].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[3].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[3].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[3].rightSide[1].tokenType = T_PLUS;
    outputPrecedenceRulesList->precedenceRuleList[3].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[3].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[4].description = "E -> E - E";
    outputPrecedenceRulesList->precedenceRuleList[4].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[4].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[4].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[4].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[4].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[4].rightSide[1].tokenType = T_MINUS;
    outputPrecedenceRulesList->precedenceRuleList[4].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[4].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[5].description = "E -> E > E";
    outputPrecedenceRulesList->precedenceRuleList[5].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[5].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[5].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[5].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[5].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[5].rightSide[1].tokenType = T_GREATER;
    outputPrecedenceRulesList->precedenceRuleList[5].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[5].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[6].description = "E -> E >= E";
    outputPrecedenceRulesList->precedenceRuleList[6].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[6].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[6].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[6].rightSide == NULL){
        return 99;
    }    
    outputPrecedenceRulesList->precedenceRuleList[6].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[6].rightSide[1].tokenType = T_GREATER_EQUAL;
    outputPrecedenceRulesList->precedenceRuleList[6].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[6].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[7].description = "E -> E < E";
    outputPrecedenceRulesList->precedenceRuleList[7].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[7].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[7].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[7].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[7].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[7].rightSide[1].tokenType = T_LESS;
    outputPrecedenceRulesList->precedenceRuleList[7].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[7].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[8].description = "E -> E <= E";
    outputPrecedenceRulesList->precedenceRuleList[8].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[8].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[8].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[8].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[8].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[8].rightSide[1].tokenType = T_LESS_EQUAL;
    outputPrecedenceRulesList->precedenceRuleList[8].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[8].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[9].description = "E -> E == E";
    outputPrecedenceRulesList->precedenceRuleList[9].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[9].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[9].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[9].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[9].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[9].rightSide[1].tokenType = T_EQUAL;
    outputPrecedenceRulesList->precedenceRuleList[9].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[9].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[10].description = "E -> E != E";
    outputPrecedenceRulesList->precedenceRuleList[10].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[10].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[10].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[10].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[10].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[10].rightSide[1].tokenType = T_NOT_EQUAL;
    outputPrecedenceRulesList->precedenceRuleList[10].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[10].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[11].description = "E -> E ?? E";
    outputPrecedenceRulesList->precedenceRuleList[11].rightSideLen = 3;
    outputPrecedenceRulesList->precedenceRuleList[11].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[11].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[11].rightSide == NULL){
        return 99;
    }
    outputPrecedenceRulesList->precedenceRuleList[11].rightSide[0].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[11].rightSide[1].tokenType = T_NIL_OP;
    outputPrecedenceRulesList->precedenceRuleList[11].rightSide[2].tokenType = T_E;
    outputPrecedenceRulesList->precedenceRuleList[11].leftSide.tokenType = T_E;

    outputPrecedenceRulesList->precedenceRuleList[12].description = "E -> id";
    outputPrecedenceRulesList->precedenceRuleList[12].rightSideLen = 1;
    outputPrecedenceRulesList->precedenceRuleList[12].rightSide = (token *) malloc(sizeof(token) * outputPrecedenceRulesList->precedenceRuleList[12].rightSideLen);
    if (outputPrecedenceRulesList->precedenceRuleList[12].rightSide == NULL){
        return 99;
    }    
    outputPrecedenceRulesList->precedenceRuleList[12].rightSide[0].tokenType = T_IDENTIFIER;
    outputPrecedenceRulesList->precedenceRuleList[12].leftSide.tokenType = T_E;

    return 0;    
}

/*Token stack*/

struct tokenStackElement
{
    token *tokenOnStack;
    struct tokenStackElement *next;
};

struct tokenStack
{
    struct tokenStackElement *top;
};

/// @brief Function returns token from the top of the stack
/// @param stack stack of tokens
/// @return token from the top of the stack
int tokenStackPush(struct tokenStack *stack, token *tokenIn)
{
    struct tokenStackElement *newElement = (struct tokenStackElement *) malloc(sizeof(struct tokenStackElement));
    token *newToken = (token *) malloc(sizeof(token));
    newElement->tokenOnStack = newToken;
    if (newElement == NULL || newElement->tokenOnStack == NULL)
    {
        printf("Malloc failed in tokenStackPush\n");
        return 99;
    }
    copy_Toke(tokenIn,newElement->tokenOnStack);
    newElement->next = stack->top;
    stack->top = newElement;

    return 0;

}



void tokenStackClear(struct tokenStack *stack)
{
    struct tokenStackElement *currentElement = stack->top;
    while (currentElement != NULL)
    {
        struct tokenStackElement *nextElement = currentElement->next;
        free (currentElement->tokenOnStack);
        free(currentElement);
        currentElement = nextElement;
    }
    free(stack);
}

enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack){
    if (stack == NULL ||stack->top == NULL || stack->top->tokenOnStack == NULL)
        printf("SEgfault in whichTypeIsOnTheStack\n");

    if (stack->top->tokenOnStack->tokenType != T_E)
        return stack->top->tokenOnStack->tokenType;
    else
        return stack->top->next->tokenOnStack->tokenType;

}


int expressionParserStart(struct precedenceRulesList *outputPrecedenceRulesList, programState *PROGRAM_STATE);

/// @brief Function returns precedence of two tokens
/// @param topOfStack token on the top of the stack
/// @param currentToken token that is being processed
/// @param precedenceTable source of precedence table
/// @return returns < > = or ' ' (space) based on precedence of two tokens
char getPrecedence(enum tokenType topOfStack, enum tokenType currentToken, char *precedenceTable);

/// @brief Get the Index In Precedence Table object
/// @param tokenType type of token whose index we want to get
/// @return unsigned int index of token in precedence table
unsigned int getIndexInPrecedenceTable(enum tokenType tokenType);

