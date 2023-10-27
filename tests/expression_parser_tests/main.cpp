#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/expression_parser.c"
#include "../../src/expression_parser.h"
#include "../../src/scanner.c"
// #include "../../src/symtable.c"
#include "../../src/str.c"
#include "../../src/parser.c"

#pragma once


TEST(getPrecedenceRuleList, Test1) {
    struct precedenceRulesList *PRL = (struct precedenceRulesList*)malloc(sizeof(struct precedenceRulesList));
    EXPECT_EQ(getPrecedenceRulesList(PRL), 0);
    EXPECT_EQ(PRL->precedenceRuleListLen, 13);
    EXPECT_EQ(PRL->precedenceRuleList[3].description,"E -> E + E");
    EXPECT_EQ(PRL->precedenceRuleList[12].leftSide.tokenType,T_E);
}

TEST(TokenToIndex, Test1){
    EXPECT_EQ(getIndexInPrecedenceTable(T_LEFT_BRACKET), 0);
    EXPECT_EQ(getIndexInPrecedenceTable(T_RIGHT_BRACKET), 1);
    EXPECT_EQ(getIndexInPrecedenceTable(T_MULTIPLICATION), 2);
    EXPECT_EQ(getIndexInPrecedenceTable(T_DIVISION), 2);
    EXPECT_EQ(getIndexInPrecedenceTable(T_PLUS), 3);
    EXPECT_EQ(getIndexInPrecedenceTable(T_MINUS), 3);
    EXPECT_EQ(getIndexInPrecedenceTable(T_LESS), 4);
    EXPECT_EQ(getIndexInPrecedenceTable(T_LESS_EQUAL), 4);
    EXPECT_EQ(getIndexInPrecedenceTable(T_GREATER), 4);
    EXPECT_EQ(getIndexInPrecedenceTable(T_GREATER_EQUAL), 4);
    EXPECT_EQ(getIndexInPrecedenceTable(T_EQUAL), 5);
    EXPECT_EQ(getIndexInPrecedenceTable(T_NOT_EQUAL), 5);
    EXPECT_EQ(getIndexInPrecedenceTable(T_NIL_OP), 6);
    EXPECT_EQ(getIndexInPrecedenceTable(T_IDENTIFIER), 7);
    EXPECT_EQ(getIndexInPrecedenceTable(T_END), 8);
}

TEST(getPrecedence,Test1){
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, '('};
    token currentToken = {T_LEFT_BRACKET, NULL, NULL, '('};
    EXPECT_EQ(getPrecedence(T_LEFT_BRACKET,T_LEFT_BRACKET, *precedenceTable), '<');
}

TEST(getPrecedence,Test2){
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, '('};
    token currentToken = {T_RIGHT_BRACKET, NULL, NULL, ')'};
    EXPECT_EQ(getPrecedence(T_LEFT_BRACKET,T_RIGHT_BRACKET, *precedenceTable), '=');    
}

TEST(getPrecedence,Test3){
    token topOfStack = {T_END, NULL, NULL, ' '};
    token currentToken = {T_END, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_END,T_END, *precedenceTable), '0');    
}

TEST(getPrecedence,Test4){
    token topOfStack = {T_NIL_OP, NULL, NULL, ' '};
    token currentToken = {T_GREATER_EQUAL, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_NIL_OP,T_GREATER_EQUAL, *precedenceTable), '>');    
}

TEST(getPrecedence,Test5){
    token topOfStack = {T_IDENTIFIER, NULL, NULL, ' '};
    token currentToken = {T_IDENTIFIER, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_IDENTIFIER,T_IDENTIFIER, *precedenceTable), '1');    
}

TEST(getPrecedence,Test6){
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, ' '};
    token currentToken = {T_END, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_LEFT_BRACKET,T_END, *precedenceTable), '1');    
}

TEST(getPrecedence,Test7){
    token topOfStack = {T_END, NULL, NULL, ' '};
    token currentToken = {T_LEFT_BRACKET, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_END,T_LEFT_BRACKET, *precedenceTable), '<');    
}

TEST(expressionParser, Test1) {
    
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token*)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = true;
    PROGRAM_STATE.lastReadToken->tokenType = T_END;

    expressionParserStart (nullptr,&PROGRAM_STATE);

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
