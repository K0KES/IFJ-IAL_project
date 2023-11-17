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
//int parse(token *activeToken, symtable *symTablePtr);
int parse(token *activeToken);

/// @brief Function gets next token and saves it in activeToken
/// @param activeToken pointer to token
/// @return returns 0 if everything went well, otherwise returns 1
int getNextToken(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool start(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool code(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool eol(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool type(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool definition(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool funcDefMid(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool functionParams(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool functionParamsN(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool functionParam(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool statements(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool statementsBlock(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool statement(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool statementOnLine(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool callOrAssign(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool assign(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool varDec(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool varDecMid(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool varDef(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool returnExpression(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool arguments(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool argumentsN(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool argument(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool argWithName(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool argumentType(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool dataType(token *activeToken);

/// @brief Function of LL grammer
/// @param activeToken pointer to token
/// @return returns true if it was syntactically correct, otherwise returns false
bool expression(token *activeToken);

#endif