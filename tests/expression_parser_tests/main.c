// #include <cstddef>
#include <gtest/gtest.h>
#include "../../src/expression_parser.c"

// Define your test cases here
TEST(ExpressionParserTest, Test1) {
    token tokenArr[] = {
        {T_IDENTIFIER, NULL, NULL, 'a'},
        {T_PLUS, NULL, NULL, '+'},
        {T_IDENTIFIER, NULL, NULL, 'b'},
        {T_EOF, NULL, NULL, 0}
    };

    EXPECT_EQ(parseExpression(tokenArr, 4, 'a'), 1);
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

    EXPECT_EQ(parseExpression(tokenArr, 13, 'a'), 0);
}

TEST(ExpressionParserTest, Test3) {
    token tokenArr[] = {
    };

    EXPECT_EQ(parseExpression(tokenArr, 0, 'a'), 1);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}