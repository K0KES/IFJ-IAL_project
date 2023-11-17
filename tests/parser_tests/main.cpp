#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//#include "../../src/expression_parser.c"
//#include "../../src/expression_parser.h"
#include "../../src/scanner.c"
#include "../../src/scanner.h"
#include "../../src/symtable.c"
#include "../../src/symtable.h"
#include "../../src/str.c"
#include "../../src/str.h"
#include "../../src/parser.c"
#include "../../src/parser.h"
#include "../../src/custom_data_types/list.c"
#include "../../src/custom_data_types/list.h"
#include "../../src/custom_data_types/hashtable.c"
#include "../../src/custom_data_types/hashtable.h"
#pragma once

TEST(Parser, Test1_0){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test1_0", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test1_2){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test1_2", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),2);
}