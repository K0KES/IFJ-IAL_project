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
//#include "../../src/str.c"
#include "../../src/parser.c"
#include "../../src/parser.h"

TEST(Parser, Test1){
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