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
    unsigned precedenceRuleListAllocatedLen;
    struct precedenceRule **precedenceRuleList;
};

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



/// @brief Function starts parsing of expression
/// @param outputPrecedenceRulesList output list of precedence rules
/// @param PROGRAM_STATE struct with program state
/// @return error code
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

/// @brief Function returns token from the top of the stack
/// @param stack stack of tokens
/// @return token from the top of the stack
int tokenStackPush(struct tokenStack *stack, token *tokenIn);

/// @brief Function copt token from t1 to t2
/// @param t1 source token
/// @param t2 destination token
void copyToken(token *t1, token *t2);

/// @brief Function deallocates memory of token stack
/// @param stack stack of tokens
void tokenStackClear(struct tokenStack *stack);

/// @brief Function returns type of token on the top of the stack and ignores T_E tokens
/// @param stack stack of tokens
/// @return type of token on the top of the stack
enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack);


/// @brief Function adds precedence rule to the list    
/// @param precedenceRulesList list of precedence rules
/// @param precedenceRule precedence rule to be added  
int addPrecedenceRuleToList(struct precedenceRulesList *precedenceRulesList, struct precedenceRule *precedenceRule);
