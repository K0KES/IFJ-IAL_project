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

TEST(tokenStackPushPopGet, Test1)
{
    struct tokenStack *tokenStack = tokenStackInit();
    token *firstToken = (token *)malloc(sizeof(token));
    firstToken->tokenType = T_END;
    token *secondToken = (token *)malloc(sizeof(token));
    secondToken->tokenType = T_E;
    token *thirdToken = (token *)malloc(sizeof(token));
    thirdToken->tokenType = T_PLUS;
    token *fourthToken = (token *)malloc(sizeof(token));
    fourthToken->tokenType = T_E;

    tokenStackPush(tokenStack, firstToken);
    tokenStackPush(tokenStack, secondToken);
    tokenStackPush(tokenStack, thirdToken);
    tokenStackPush(tokenStack, fourthToken);

    EXPECT_EQ(tokenStackGet(tokenStack, 0)->tokenType, T_E);
    EXPECT_EQ(tokenStackGet(tokenStack, 1)->tokenType, T_PLUS);
    EXPECT_EQ(tokenStackGet(tokenStack, 2)->tokenType, T_E);
    EXPECT_EQ(tokenStackGet(tokenStack, 3)->tokenType, T_END);
    EXPECT_EQ(tokenStackGet(tokenStack, 4), nullptr);
    EXPECT_EQ(tokenStackPop(tokenStack, 3), 0);

    EXPECT_EQ(tokenStackPop(tokenStack, 2), 1);
    free(firstToken);
    free(secondToken);
    free(thirdToken);
    free(fourthToken);
    free(tokenStack);
    // sleep (1);
}

TEST(getFromStackTop, Test1)
{
    struct tokenStack *tokenStack = tokenStackInit();
    token *firstToken = (token *)malloc(sizeof(token));
    firstToken->tokenType = T_END;
    tokenStackPush(tokenStack, firstToken);
    EXPECT_EQ(whichTypeIsOnTheStack(tokenStack), T_END);
    token *secondToken = (token *)malloc(sizeof(token));
    secondToken->tokenType = T_E;
    tokenStackPush(tokenStack, secondToken);
    EXPECT_EQ(whichTypeIsOnTheStack(tokenStack), T_END);
    token *thirdToken = (token *)malloc(sizeof(token));
    thirdToken->tokenType = T_PLUS;
    tokenStackPush(tokenStack, thirdToken);
    EXPECT_EQ(whichTypeIsOnTheStack(tokenStack), T_PLUS);
    token *fourthToken = (token *)malloc(sizeof(token));
    fourthToken->tokenType = T_E;
    tokenStackPush(tokenStack, fourthToken);
    EXPECT_EQ(whichTypeIsOnTheStack(tokenStack), T_PLUS);
    tokenStackClear(tokenStack);
    free(firstToken);
    free(secondToken);
    free(thirdToken);
    free(fourthToken);
    // free(fourthToken);
}

TEST(addPrecedenceRuleToList, Test1)
{
    struct precedenceRuleList *precedenceRuleList = precedenceRuleListInit();

    struct precedenceRule *precedenceRule1 = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
    precedenceRule1->leftSide.tokenType = T_E;
    precedenceRule1->rightSideLen = 1;
    precedenceRule1->rightSide = (token *)malloc(sizeof(token));
    precedenceRule1->rightSide[0].tokenType = T_IDENTIFIER;

    addPrecedenceRuleToList(precedenceRuleList, precedenceRule1);
    EXPECT_EQ(precedenceRuleList->precedenceRuleListLen, 1);
    EXPECT_EQ(precedenceRuleList->precedenceRuleListAllocatedLen, 1);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[0]->leftSide.tokenType, T_E);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[0]->rightSideLen, 1);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);

    struct precedenceRule *precedenceRule2 = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
    precedenceRule2->leftSide.tokenType = T_E;
    precedenceRule2->rightSideLen = 3;
    precedenceRule2->rightSide = (token *)malloc(sizeof(token) * 3);
    precedenceRule2->rightSide[0].tokenType = T_E;
    precedenceRule2->rightSide[1].tokenType = T_PLUS;
    precedenceRule2->rightSide[2].tokenType = T_E;

    addPrecedenceRuleToList(precedenceRuleList, precedenceRule2);
    EXPECT_EQ(precedenceRuleList->precedenceRuleListLen, 2);
    EXPECT_EQ(precedenceRuleList->precedenceRuleListAllocatedLen, 2);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[1]->leftSide.tokenType, T_E);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[1]->rightSideLen, 3);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[1]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[1]->rightSide[1].tokenType, T_PLUS);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[1]->rightSide[2].tokenType, T_E);

    struct precedenceRule *precedenceRule3 = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
    precedenceRule3->leftSide.tokenType = T_E;
    precedenceRule3->rightSideLen = 3;
    precedenceRule3->rightSide = (token *)malloc(sizeof(token) * 3);
    precedenceRule3->rightSide[0].tokenType = T_E;
    precedenceRule3->rightSide[1].tokenType = T_MINUS;
    precedenceRule3->rightSide[2].tokenType = T_E;

    addPrecedenceRuleToList(precedenceRuleList, precedenceRule3);
    EXPECT_EQ(precedenceRuleList->precedenceRuleListLen, 3);
    EXPECT_EQ(precedenceRuleList->precedenceRuleListAllocatedLen, 4);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[2]->leftSide.tokenType, T_E);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[2]->rightSideLen, 3);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[2]->rightSide[1].tokenType, T_MINUS);
    EXPECT_EQ(precedenceRuleList->precedenceRuleList[2]->rightSide[2].tokenType, T_E);

    free(precedenceRuleList->precedenceRuleList[0]->rightSide);
    free(precedenceRuleList->precedenceRuleList[0]);
    free(precedenceRuleList->precedenceRuleList[1]->rightSide);
    free(precedenceRuleList->precedenceRuleList[1]);
    free(precedenceRuleList->precedenceRuleList[2]->rightSide);
    free(precedenceRuleList->precedenceRuleList[2]);
    free(precedenceRuleList->precedenceRuleList);
    free(precedenceRuleList);
}

TEST(TokenToIndex, Test1)
{
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

TEST(getPrecedence, Test1)
{
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, '('};
    token currentToken = {T_LEFT_BRACKET, NULL, NULL, '('};
    EXPECT_EQ(getPrecedence(T_LEFT_BRACKET, T_LEFT_BRACKET, *precedenceTable), '<');
}

TEST(getPrecedence, Test2)
{
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, '('};
    token currentToken = {T_RIGHT_BRACKET, NULL, NULL, ')'};
    EXPECT_EQ(getPrecedence(T_LEFT_BRACKET, T_RIGHT_BRACKET, *precedenceTable), '=');
}

TEST(getPrecedence, Test3)
{
    token topOfStack = {T_END, NULL, NULL, ' '};
    token currentToken = {T_END, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_END, T_END, *precedenceTable), '0');
}

TEST(getPrecedence, Test4)
{
    token topOfStack = {T_NIL_OP, NULL, NULL, ' '};
    token currentToken = {T_GREATER_EQUAL, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_NIL_OP, T_GREATER_EQUAL, *precedenceTable), '>');
}

TEST(getPrecedence, Test5)
{
    token topOfStack = {T_IDENTIFIER, NULL, NULL, ' '};
    token currentToken = {T_IDENTIFIER, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_IDENTIFIER, T_IDENTIFIER, *precedenceTable), '1');
}

TEST(getPrecedence, Test6)
{
    token topOfStack = {T_LEFT_BRACKET, NULL, NULL, ' '};
    token currentToken = {T_END, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_LEFT_BRACKET, T_END, *precedenceTable), '1');
}

TEST(getPrecedence, Test7)
{
    token topOfStack = {T_END, NULL, NULL, ' '};
    token currentToken = {T_LEFT_BRACKET, NULL, NULL, ' '};
    EXPECT_EQ(getPrecedence(T_END, T_LEFT_BRACKET, *precedenceTable), '<');
}

TEST(expressionParser, Test1)
{

    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = true;
    PROGRAM_STATE.lastReadToken->tokenType = T_ARROW;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test1.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 42);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_ARROW);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);

    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserToIdTest, Test1)
{

    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test1.txt", "r", stdin);

    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);

    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserPlusTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = true;
    PROGRAM_STATE.lastReadToken->tokenType = T_IDENTIFIER;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test2.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_COMMA);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_PLUS);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserBracketTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_IDENTIFIER;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test3.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // EXPECT_EQ (PROGRAM_STATE.lastReadToken->tokenType,T_COMMA);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_LEFT_BRACKET);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[1].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[2].tokenType, T_RIGHT_BRACKET);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserMinusTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test4.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_MINUS);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserMultiplicationTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test5.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_MULTIPLICATION);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserDivisionTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test6.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_DIVISION);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserLessTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test7.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_LESS);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserLessEqualTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test8.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_LESS_EQUAL);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserGreaterTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test9.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_GREATER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserGreaterEqualTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test10.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_GREATER_EQUAL);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserEqualTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test11.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_EQUAL);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserNotEqualTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test12.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    // printf("%d\n",PROGRAM_STATE.lastReadToken->tokenType);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);

    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_NOT_EQUAL);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserNilTest, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test13.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);
    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_NIL_OP);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserMoreComplexEpressions, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = true;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test14.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);
    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[1].tokenType, T_PLUS);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[2].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[0].tokenType, T_LEFT_BRACKET);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[1].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[2].tokenType, T_RIGHT_BRACKET);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserMoreComplexEpressions, Test2)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test15.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);
    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[1].tokenType, T_MULTIPLICATION);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[2].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[1].tokenType, T_PLUS);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[2].tokenType, T_E);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserMoreComplexEpressions, Test3)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test16.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);
    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_INT);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_INT);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_INT);

    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[1].tokenType, T_PLUS);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[2].tokenType, T_E);

    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[0].tokenType, T_LEFT_BRACKET);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[1].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[2].tokenType, T_RIGHT_BRACKET);

    EXPECT_EQ(pRL1->precedenceRuleList[5]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[5]->rightSide[1].tokenType, T_MULTIPLICATION);
    EXPECT_EQ(pRL1->precedenceRuleList[5]->rightSide[2].tokenType, T_E);

    EXPECT_EQ(pRL1->precedenceRuleList[6]->rightSide[0].tokenType, T_INT);

    EXPECT_EQ(pRL1->precedenceRuleList[7]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[7]->rightSide[1].tokenType, T_DIVISION);
    EXPECT_EQ(pRL1->precedenceRuleList[7]->rightSide[2].tokenType, T_E);

    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserMoreComplexEpressions, Test4)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test21.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    EXPECT_EQ(PROGRAM_STATE.isLastReadTokenValid, true);
    EXPECT_EQ(PROGRAM_STATE.lastReadToken->tokenType, T_EOF);
    EXPECT_EQ(pRL1->precedenceRuleList[0]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[1]->rightSide[0].tokenType, T_IDENTIFIER);
    EXPECT_EQ(pRL1->precedenceRuleList[2]->rightSide[0].tokenType, T_INT);

    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[1].tokenType, T_MINUS);
    EXPECT_EQ(pRL1->precedenceRuleList[3]->rightSide[2].tokenType, T_E);

    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[0].tokenType, T_LEFT_BRACKET);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[1].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[4]->rightSide[2].tokenType, T_RIGHT_BRACKET);

    EXPECT_EQ(pRL1->precedenceRuleList[5]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[5]->rightSide[1].tokenType, T_PLUS);
    EXPECT_EQ(pRL1->precedenceRuleList[5]->rightSide[2].tokenType, T_E);

    EXPECT_EQ(pRL1->precedenceRuleList[6]->rightSide[0].tokenType, T_LEFT_BRACKET);
    EXPECT_EQ(pRL1->precedenceRuleList[6]->rightSide[1].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[6]->rightSide[2].tokenType, T_RIGHT_BRACKET);

    EXPECT_EQ(pRL1->precedenceRuleList[7]->rightSide[0].tokenType, T_IDENTIFIER);

    EXPECT_EQ(pRL1->precedenceRuleList[8]->rightSide[0].tokenType, T_E);
    EXPECT_EQ(pRL1->precedenceRuleList[8]->rightSide[1].tokenType, T_NIL_OP);
    EXPECT_EQ(pRL1->precedenceRuleList[8]->rightSide[2].tokenType, T_E);

    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test1)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test17.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 2);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test2)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test18.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 2);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test3)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test19.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 2);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test4)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_LEFT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test20.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test5)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = true;
    PROGRAM_STATE.lastReadToken->tokenType = T_RIGHT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test22.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 2);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test6)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_RIGHT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test22.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 2);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test7)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_RIGHT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test23.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 0);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

TEST(expressionParserSintaxError, Test8)
{
    programState PROGRAM_STATE;
    PROGRAM_STATE.lastReadToken = (token *)malloc(sizeof(token));
    PROGRAM_STATE.isLastReadTokenValid = false;
    PROGRAM_STATE.lastReadToken->tokenType = T_RIGHT_BRACKET;

    struct precedenceRuleList *pRL1 = precedenceRuleListInit();

    freopen("tests/expression_parser_tests/test24.txt", "r", stdin);
    EXPECT_EQ(expressionParserStart(pRL1, &PROGRAM_STATE), 2);
    free(PROGRAM_STATE.lastReadToken);
    precedenceRuleListClear(pRL1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
