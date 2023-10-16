#include <gtest/gtest.h>
#include <stdlib.h>
#include "../../src/expression_parser.c"

// Define your test cases here
TEST(ExpressionParserTest, Test1) {
    token tokenArr[] = {
        {T_IDENTIFIER, NULL, NULL, 'a'},
        {T_PLUS, NULL, NULL, '+'},
        {T_IDENTIFIER, NULL, NULL, 'b'},
        {EP_$, NULL, NULL, 0}
    };
    struct precedenceRulesList *outputPrecedenceRulesList = (struct precedenceRulesList *) malloc(sizeof(struct precedenceRulesList));

    EXPECT_EQ(parseExpression(tokenArr, 4, outputPrecedenceRulesList), 1);
}

TEST(ExpressionParserTest, Test2) {
    token tokenArr[] = {
        {T_IDENTIFIER, NULL, NULL, 'a'},
        {T_PLUS, NULL, NULL, '+'},
        {T_IDENTIFIER, NULL, NULL, 'b'},
        {T_MINUS, NULL, NULL, '-'},
        {T_IDENTIFIER, NULL, NULL, 'c'},
        {T_MULTIPLICATION, NULL, NULL, '*'},
        {T_IDENTIFIER, NULL, NULL, 'd'},
        {T_DIVISION, NULL, NULL, '/'},
        {T_IDENTIFIER, NULL, NULL, 'e'},
        {T_GREATER, NULL, NULL, '>'},
        {T_LEFT_BRACKET, NULL, NULL, '('},
        {T_IDENTIFIER, NULL, NULL, 'f'},
        {T_RIGHT_BRACKET, NULL, NULL, ')'}
    };
    struct precedenceRulesList *outputPrecedenceRulesList = (struct precedenceRulesList *) malloc(sizeof(struct precedenceRulesList));

    EXPECT_EQ(parseExpression(tokenArr, 13, outputPrecedenceRulesList), 0);
}

TEST(ExpressionParserTest, Test3) {
    token tokenArr[] = {
    };
    struct precedenceRulesList *outputPrecedenceRulesList = (struct precedenceRulesList *) malloc(sizeof(struct precedenceRulesList));

    EXPECT_EQ(parseExpression(tokenArr, 0, outputPrecedenceRulesList), 1);
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
    EXPECT_EQ(getIndexInPrecedenceTable(EP_$), 8);
}

TEST(getPrecedence,Test1){
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, '('};
    token currentToken = {T_LEFT_BRACKET, NULL, NULL, '('};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '<');
}

TEST(getPrecedence,Test2){
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, '('};
    token currentToken = {T_RIGHT_BRACKET, NULL, NULL, ')'};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '=');    
}

TEST(getPrecedence,Test3){
    token topOfStack = {EP_$, NULL, NULL, ' '};
    token currentToken = {EP_$, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '0');    
}

TEST(getPrecedence,Test4){
    token topOfStack = {T_NIL_OP, NULL, NULL, ' '};
    token currentToken = {T_GREATER_EQUAL, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '>');    
}

TEST(getPrecedence,Test5){
    token topOfStack = {T_IDENTIFIER, NULL, NULL, ' '};
    token currentToken = {T_IDENTIFIER, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '1');    
}

TEST(getPrecedence,Test6){
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, ' '};
    token currentToken = {EP_$, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '1');    
}

TEST(getPrecedence,Test7){
    token topOfStack = {EP_$, NULL, NULL, ' '};
    token currentToken = {T_LEFT_BRACKET, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(topOfStack, currentToken, *precedenceTable), '<');    
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
