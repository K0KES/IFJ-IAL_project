#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#pragma once

/// @brief Function gets next token and saves it in global var
/// @param activeToken global var
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
bool statement(token *activeToken);

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
