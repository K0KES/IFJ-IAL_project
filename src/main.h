#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "generator.h"
#include "symtable.h"
#include "./custom_data_types/list.h"
#include "error.h"

#pragma once

typedef struct {
    list *tokenQueue;
    generator *gen;
    symtable *symTable;
    token *activeToken;
    enum tokenType expParserReturnType;
} programState;

programState *programStateInit();

void programStateFree(programState *state);

int main(int argc, char const *argv[]);