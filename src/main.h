#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "generator.h"
#include "symtable.h"
#include "list.h"
#include "error.h"

#pragma once

typedef struct {
    list *tokenQueue;
    generator *gen;
    symtable *symTable;
    token *activeToken;
    enum tokenType expParserReturnType;
    bool expParserReturnTypeNullable;
    bool changeToDouble;
} programState;

programState *programStateInit();

void programStateFree(programState *state);

int main(int argc, char const *argv[]);