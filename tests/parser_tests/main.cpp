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

TEST(Parser, Test1_0_functions){
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

TEST(Parser, Test1_2_functions){
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

TEST(Parser, Test2_0_var_def){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test2_0", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test3_0_while_if_statements){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test3_0", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test4_0_function_with_statements){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test4_0", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test5_0_build_in_functions){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test5_0", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test6_kod_ze_zadani_1){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test_zadani_1", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test7_kod_ze_zadani_2){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test_zadani_2", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}

TEST(Parser, Test8_kod_ze_zadani_3){
    token *activeToken = (token *)malloc(sizeof(token));
    activeToken->value = (string*)malloc(sizeof(string));
    strInit(activeToken->value);
    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;

    symtable *symTable = symtableInit();

    freopen("tests/parser_tests/test_inputs/test_zadani_3", "r", stdin);

    EXPECT_EQ(parse(activeToken,symTable),0);
}