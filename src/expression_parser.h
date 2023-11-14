#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
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
struct precedenceRuleList
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

struct tokenQueueElement
{
    token *tokenInQueue;
    struct tokenQueueElement* next;
};

struct tokenQueue
{
    struct tokenQueueElement* first;
    struct tokenQueueElement* last;

};

/// @brief Add token to the end of the queue
/// @param tQ Pointer to queue
/// @param tokenIn Pointer to token, that should be added
/// @return error code
int addLastToQueue(struct tokenQueue* tQ, token* tokenIn);

/// @brief Returns first token from queue
/// @param tQ Pointer to queue
/// @return Pointer to token, that is first in queue
token* getFirstFromQueue(struct tokenQueue* tQ);

/// @brief Remove first token from queue
/// @param tQ Pointer to queue
/// @return Error code
int popFirstFromQueue(struct tokenQueue* tQ);

/// @brief Function starts parsing of expression
/// @param outputPrecedenceRulesList output list of precedence rules
/// @param PROGRAM_STATE struct with program state
/// @return error code
int expressionParserStart(struct precedenceRuleList *outputPrecedenceRulesList, programState *PROGRAM_STATE);

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

struct tokenStack* tokenStackInit();

/// @brief Function copt token from t1 to t2
/// @param t1 source token
/// @param t2 destination token
void copyToken(token *t1, token *t2);

/// @brief Function deallocates memory of token stack
/// @param stack stack of tokens
void tokenStackClear(struct tokenStack *stack);

/// @brief Function removes token from the top of the stack 
/// @param stack Pointer to stack of tokens
/// @param numberOfPops Nu,ber of tokens that should be removed from the stack
int tokenStackPop(struct tokenStack *stack, unsigned numberOfPops);


/// @brief Get you token onf certain location form stack
/// @param stack Pointer to stack
/// @param location Location (first index is 0)
/// @return REturns pointer on token, that is on the location, in case of failure returns NULL
token* tokenStackGet(struct tokenStack *stack, unsigned location);

/// @brief Function returns type of token on the top of the stack and ignores T_E tokens
/// @param stack stack of tokens
/// @return type of token on the top of the stack
enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack);

/// @brief Function adds precedence rule to the list    
/// @param precedenceRuleList list of precedence rules
/// @param precedenceRule precedence rule to be added  
int addPrecedenceRuleToList(struct precedenceRuleList *precedenceRuleList, struct precedenceRule *precedenceRule);

/// @brief Decides if expression parser can parse this token
/// @param activeToken Token to decide
/// @return True if token is accepted
bool isTokenTypeAccepted (token *activeToken);

/// @brief Just adds information to token that are necessary for token scanner
/// @param T Token to set up
/// @return Returns 0 if everything is ok, 1 if there is an error
int setUpActiveToken(token* T);