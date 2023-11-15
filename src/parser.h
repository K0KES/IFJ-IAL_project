#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#pragma once

/// @brief Function initialize memory for one active token
/// @param activeToken pointer to token
/// @return returns 0 if everything went well, otherwise returns 99
int tokenInit(token *activeToken);

/// @brief Function clear memory of a token
/// @param activeToken pointer to token
/// @return returns 0 if everything went well, otherwise returns 99
int tokenFree(token *activeToken);

/// @brief Function that starts the parser
/// @param activeToken pointer to token
/// @param symTablePtr pointer to symtable
/// @return returns 0 if was parsed succesfully, otherwise returns 2
int parse(token *activeToken, symtable *symTablePtr);

/// @brief Function gets next token and saves it in activeToken
/// @param activeToken pointer to token
/// @return returns 0 if everything went well, otherwise returns error code
int getNextToken(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool start(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool code(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool eol(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool type(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool definition(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool funcDefMid(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool functionParams(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool functionParamsN(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool functionParam(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool statements(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool statementsBlock(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool statement(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool statementOnLine(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool callOrAssign(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool varDec(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool varDecMid(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool varDef(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool returnExpression(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool arguments(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool argumentsN(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool argument(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool argWithName(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool argumentType(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool dataType(token *activeToken);

/// @brief Function 
/// @return returns true if is okok
bool expression(token *activeToken);

#endif