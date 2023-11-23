#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//#include "../../src/expression_parser.c"
//#include "../../src/expression_parser.h"

#include "../../src/expression_parser.h"
#include "../../src/expression_parser.c"
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
#include "../../src/generator.c"
#include "../../src/generator.h"
#include "../../src/error.c"
#include "../../src/error.h"
#include "../../src/main.h"
#include "../../src/main.c"
#pragma once

TEST(Parser, Test1_0_functions){
    printf("TESDT\n");
    programState *programState = programStateInit(); 
    
    freopen("tests/parser_tests/test_inputs/test1_0", "r", stdin);

    EXPECT_EXIT(parse(programState),testing::ExitedWithCode(ERR_MISSING_TYPE), "");

    generatorGenerateOutput(programState->gen);
    programStateFree(programState);
}
/*
TEST(Parser, Test1_2_functions){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test1_2", "r", stdin);

    EXPECT_EQ(parse(programState),2);
}

TEST(Parser, Test2_0_var_def){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test2_0", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}

TEST(Parser, Test3_0_while_if_statements){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);
    freopen("tests/parser_tests/test_inputs/test3_0", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}

TEST(Parser, Test4_0_function_with_statements){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test4_0", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}

TEST(Parser, Test5_0_build_in_functions){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test5_0", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}

TEST(Parser, Test6_kod_ze_zadani_1){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test_zadani_1", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}

TEST(Parser, Test7_kod_ze_zadani_2){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test_zadani_2", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}

TEST(Parser, Test8_kod_ze_zadani_3){
    programState *programState = programStateInit(); 
    parse(programState);
    generatorGenerateOutput(programState->gen);
    programStateFree(programState);

    freopen("tests/parser_tests/test_inputs/test_zadani_3", "r", stdin);

    EXPECT_EQ(parse(programState),0);
}*/