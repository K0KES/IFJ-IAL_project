#include "scanner.h"
#include "symtable.h"
#include "generator.h"
#include "main.h"
#include "list.h"
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "error.h"
#pragma once

// Length of array of accepted token types
unsigned int acceptedTokenTypesLength = 29;

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
    T_STRING,

    KW_NIL,

    T_NOT,

    KW_READDOUBLE,
    KW_READINT,
    KW_READSTRING,
    KW_INT_TO_DOUBLE,
    KW_DOUBLE_TO_INT,
    KW_LENGTH,
    KW_SUBSTRING,
    KW_ORD,
    KW_CHR};

char precedenceTable[10][10] = {
    {'<', '=', '<', '<', '<', '<', '<','>','<', '1'},
    {'1', '>', '>', '>', '>', '>', '>','>','1', '>'},
    {'<', '>', '>', '>', '>', '>', '>','<','<', '>'},
    {'<', '>', '<', '>', '>', '>', '>','<','<', '>'},
    {'<', '>', '<', '<', '>', '>', '>','<','<', '>'},
    {'<', '>', '<', '<', '>', '>', '>','<','<', '>'},
    {'<', '>', '<', '<', '>', '<', '<','<','<', '>'},
    {'>', '>', '>', '>', '>', '>', '>','>','>', '>'},
    {'1', '>', '>', '>', '>', '>', '>','>','1', '>'},
    {'<', '1', '<', '<', '<', '<', '<','<','<', '0'}};

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
    struct tokenQueueElement *next;
};

struct tokenQueue
{
    struct tokenQueueElement *first;
    struct tokenQueueElement *last;
};

/// @brief Add token to the end of the queue
/// @param tQ Pointer to queue
/// @param tokenIn Pointer to token, that should be added
/// @return error code
int addLastToQueue(struct tokenQueue *tQ, token *tokenIn);

/// @brief Returns first token from queue
/// @param tQ Pointer to queue
/// @return Pointer to token, that is first in queue
token *getFirstFromQueue(struct tokenQueue *tQ);

/// @brief Returns last token from queue
/// @param tQ Pointer to queue
/// @return Pointer to token, that is last in queue
token *getLastFromQueue(struct tokenQueue *tQ);

/// @brief Remove last token from queue
/// @param tQ Pointer to queue
/// @return Pointer to token, that is last in queue
token *popLastFromQueue(struct tokenQueue *tQ);

/// @brief Remove first token from queue
/// @param tQ Pointer to queue
/// @return Error code
int popFirstFromQueue(struct tokenQueue *tQ);

/// @brief Function starts parsing of expression
/// @param outputPrecedenceRulesList output list of precedence rules
/// @param PROGRAM_STATE struct with program state
/// @return error code
int expressionParserStart(programState *PS);

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

struct tokenStack *tokenStackInit();

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
token *tokenStackGet(struct tokenStack *stack, unsigned location);

/// @brief Function returns type of token on the top of the stack and ignores T_E tokens
/// @param stack stack of tokens
/// @return type of token on the top of the stack
enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack);

/// @brief Decides if expression parser can parse this token
/// @param activeToken Token to decide
/// @return True if token is accepted
bool isTokenTypeAccepted(token *activeToken);

/// @brief Just adds information to token that are necessary for token scanner
/// @param T Token to set up
/// @return Returns 0 if everything is ok, 1 if there is an error
int setUpActiveToken(token *T);

/// @brief Free all memmory used by token stack
/// @param stack Pointer to token stack
void tokenStackClear(struct tokenStack *stack);

/// @brief Check is the tokens on the top of the stack are valid
/// @param stack
/// @return Returns error code
int checkTokensOnTopOfTheStack(struct tokenStack *stack);

/// @brief Function for classification of token types
/// @param tokenType Input token type
/// @return Returns 1 if token type is operator, returns 2 if token type is left bracket, return 3 in token type is right_bracket else return 0
int isTokenTypeOperatorLike(enum tokenType tokenType);

/// @brief Function for classification of token types
/// @param tokenType Input token type
/// @return Returns false if token can't be function, else returns true
int isTokenFunction(enum tokenType tokenType);

int dealWithFunction(struct tokenStack *stack, struct tokenQueue *queue, programState *PS);
