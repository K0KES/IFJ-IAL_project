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
        {T_IDENTIFIER, NULL, NULL, 'f'}
    };

    EXPECT_EQ(parseExpression(tokenArr, 11, 'a'), 0);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}