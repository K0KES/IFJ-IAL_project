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

TEST(tokenStackPushPopGet, Test1){
    struct tokenStack *tokenStack = (struct tokenStack *)malloc(sizeof(tokenStack));
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
    EXPECT_EQ(tokenStackGet(tokenStack,0)->tokenType,T_E);
    EXPECT_EQ(tokenStackGet(tokenStack,1)->tokenType,T_PLUS);
    EXPECT_EQ(tokenStackGet(tokenStack,2)->tokenType,T_E);
    EXPECT_EQ(tokenStackGet(tokenStack,3)->tokenType,T_END);
    EXPECT_EQ(tokenStackGet(tokenStack,4),nullptr);
    EXPECT_EQ(tokenStackPop(tokenStack,3),0);
    EXPECT_EQ(tokenStackPop(tokenStack,2),1);
}




TEST(getFromStackTop,Test1){
    struct tokenStack *tokenStack = (struct tokenStack *)malloc(sizeof(tokenStack));
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
    free(tokenStack);
    free(firstToken);
    free(secondToken);
    free(thirdToken);
    free(fourthToken);

}

TEST (addPrecedenceRuleToList, Test1){
    struct precedenceRuleList *precedenceRuleList = (struct precedenceRuleList *)malloc(sizeof(struct precedenceRuleList));
    precedenceRuleList->precedenceRuleListLen = 0;
    precedenceRuleList->precedenceRuleListAllocatedLen = 1;
    precedenceRuleList->precedenceRuleList = (struct precedenceRule **)malloc(sizeof(struct precedenceRule *));
    

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

    struct precedenceRuleList pRL1;
    pRL1.precedenceRuleListLen = 0;
    pRL1.precedenceRuleListAllocatedLen = 1;
    pRL1.precedenceRuleList = (struct precedenceRule**) malloc(sizeof(struct precedenceRule*));


    expressionParserStart (&pRL1,&PROGRAM_STATE);
    free (PROGRAM_STATE.lastReadToken);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
