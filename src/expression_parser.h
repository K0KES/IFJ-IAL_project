#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Length of array of accepted token types
unsigned int acceptedTokenTypesLength = 14;

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
    T_NIL_OP};

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

/* Precedence rules */

/// @brief Structure that stores one precedence rule
struct precedenceRule
{
    // Basic description that is meant to be used only for debugging
    char *description;
    // Length of token list
    unsigned tokenListLen;
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

/*Token stack*/

struct tokenStackElement
{
    token *tokenValue;
    struct tokenStackElement *next;
};

struct tokenStack
{
    struct tokenStackElement *top;
};

/// @brief Function returns token from the top of the stack
/// @param stack stack of tokens
/// @return token from the top of the stack
void tokenStackPush(struct tokenStack *stack, token *tokenValue)
{
    struct tokenStackElement *newElement = (struct tokenStackElement *)malloc(sizeof(struct tokenStackElement));
    newElement->tokenValue = tokenValue;
    newElement->next = stack->top;
    stack->top = newElement;
}


int parseExpression(token *tokenArr, unsigned tokenArrLength, struct precedenceRulesList *outputPrecedenceRulesList);


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
