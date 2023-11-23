#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symtable.h"
#include "error.h"
#include "generator.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#pragma once

/// @brief Function initialize memory for one active token
/// @return returns pointer to activeToken
token *tokenInit();

/// @brief Function clear memory of a token
/// @param activeToken pointer to token
/// @return return
void tokenFree(token *activeToken );

/// @brief Function that starts the parser
/// @param programState pointer to programState
/// @return returns 0 if was parsed succesfully, otherwise returns 2
int parse(programState *programState);

/// @brief Function gets next token and saves it in activeToken
/// @return returns 0 if everything went well, otherwise returns 1
int getNextToken();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool start();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool code();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool eol();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool type();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool definition();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool funcDefMid();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool functionParams();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool functionParamsN();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool functionParam();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool statements();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool statementsBlock();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool statement();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool statementOnLine();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool callOrAssign();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool assign();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool varDec();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool varDecMid();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool varDef();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool returnExpression();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool arguments();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool argumentsN();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool argument();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool argWithName();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool argumentType();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool dataType();

/// @brief Function of LL grammer
/// @return returns true if it was syntactically correct, otherwise returns false
bool expression();

#endif