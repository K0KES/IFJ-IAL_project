#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "generator.h"
#include "symtable.h"
#include "list.h"
#include "error.h"

#pragma once

/// @brief Shared structure for all parts of the program
typedef struct{
    list *tokenQueue;
    generator *gen;
    symtable *symTable;
    token *activeToken;
    enum tokenType expParserReturnType;
    bool expParserReturnTypeNullable;
    bool changeToDouble;
} programState;

/// @brief Initializes program state
/// @return Initialized program state
programState *programStateInit();

/// @brief Cleans up program state
/// @param state Initialized program state
void programStateFree(programState *state);

/// @brief Main function
/// @param argc Number of arguments
/// @param argv Arguments
/// @return Exit code
int main(int argc, char const *argv[]);