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
bool start();

/// @brief Function 
/// @return returns true if is okok
bool code();

/// @brief Function 
/// @return returns true if is okok
bool eol();

/// @brief Function 
/// @return returns true if is okok
bool type();

/// @brief Function 
/// @return returns true if is okok
bool definition();

/// @brief Function 
/// @return returns true if is okok
bool funcDefMid();

/// @brief Function 
/// @return returns true if is okok
bool functionParams();

/// @brief Function 
/// @return returns true if is okok
bool functionParamsN();

/// @brief Function 
/// @return returns true if is okok
bool functionParam();

/// @brief Function 
/// @return returns true if is okok
bool statements();

/// @brief Function 
/// @return returns true if is okok
bool statement();

/// @brief Function 
/// @return returns true if is okok
bool callOrAssign();

/// @brief Function 
/// @return returns true if is okok
bool varDec();

/// @brief Function 
/// @return returns true if is okok
bool varDecMid();

/// @brief Function 
/// @return returns true if is okok
bool varDef();

/// @brief Function 
/// @return returns true if is okok
bool returnExpression();

/// @brief Function 
/// @return returns true if is okok
bool arguments();

/// @brief Function 
/// @return returns true if is okok
bool argumentsN();

/// @brief Function 
/// @return returns true if is okok
bool argument();

/// @brief Function 
/// @return returns true if is okok
bool argWithName();

/// @brief Function 
/// @return returns true if is okok
bool argumentType();

/// @brief Function 
/// @return returns true if is okok
bool dataType();

/// @brief Function 
/// @return returns true if is okok
bool expression();
