#include "expression_parser.h"

void copy_Token(token *t1, token *t2)
{
    t2->lastChar = t1->lastChar;
    t2->position = t1->position;
    t2->tokenType = t1->tokenType;
    t2->value = t2->value;
}

token *tokenStackGet(struct tokenStack *stack, unsigned location)
{
    struct tokenStackElement *tSE = stack->top;
    for (unsigned i = 0; i < location; i++)
    {
        if (tSE != NULL && tSE->next != NULL)
        {
            tSE = tSE->next;
        }
        else
        {
            printf("Error: stack is too small!\n");
            return (NULL);
        }
    }
    return (tSE->tokenOnStack);
}

enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack)
{
    if (stack == NULL || stack->top == NULL || stack->top->tokenOnStack == NULL)
        printf("Segfault in whichTypeIsOnTheStack\n");

    if (stack->top->tokenOnStack->tokenType != T_E)
        return stack->top->tokenOnStack->tokenType;
    else
        return stack->top->next->tokenOnStack->tokenType;
}

void tokenStackClear(struct tokenStack *stack)
{
    if (stack == NULL || stack->top == NULL)
    {
        return;
    }

    struct tokenStackElement *currentElement = stack->top;
    while (currentElement != NULL)
    {
        struct tokenStackElement *nextElement = currentElement->next;
        free(currentElement->tokenOnStack);
        currentElement->tokenOnStack = NULL;
        free(currentElement);

        currentElement = nextElement;
    }
    free(stack);
}

int tokenStackPush(struct tokenStack *stack, token *tokenIn)
{
    struct tokenStackElement *newElement = (struct tokenStackElement *)malloc(sizeof(struct tokenStackElement));
    token *newToken = (token *)malloc(sizeof(token));
    newElement->tokenOnStack = newToken;
    if (newElement == NULL || newElement->tokenOnStack == NULL)
    {
        printf("Malloc failed in tokenStackPush\n");
        return 99;
    }
    newElement->next = NULL;
    copy_Token(tokenIn, newElement->tokenOnStack);
    newElement->next = stack->top;
    stack->top = newElement;

    return 0;
}

struct tokenStack *tokenStackInit()
{
    struct tokenStack *tokenStack = (struct tokenStack *)malloc(sizeof(struct tokenStack));
    if (tokenStack == NULL)
    {
        printf("Malloc failed in tokenStackInit\n");
        return NULL;
    }
    tokenStack->top = NULL;
    return tokenStack;
}

int tokenStackPop(struct tokenStack *stack, unsigned numberOfPops)
{
    for (unsigned i = 0; i < numberOfPops; i++)
    {
        if (stack->top != NULL)
        {
            struct tokenStackElement *nextElement = stack->top->next;
            free(stack->top->tokenOnStack);
            free(stack->top);
            stack->top = nextElement;
        }
        else
        {
            printf("Error: stack is empty!\n");
            return 1;
        }
    }
    return 0;
}

int addPrecedenceRuleToList(struct precedenceRuleList *precedenceRuleList, struct precedenceRule *precedenceRule)
{
    if (precedenceRuleList->precedenceRuleListLen == precedenceRuleList->precedenceRuleListAllocatedLen)
    {
        precedenceRuleList->precedenceRuleListAllocatedLen *= 2;
        precedenceRuleList->precedenceRuleList = (struct precedenceRule **)realloc(precedenceRuleList->precedenceRuleList, precedenceRuleList->precedenceRuleListAllocatedLen * sizeof(struct precedenceRule *));
        if (precedenceRuleList->precedenceRuleList == NULL)
        {
            return 99;
        }
    }
    precedenceRuleList->precedenceRuleList[precedenceRuleList->precedenceRuleListLen] = precedenceRule;
    precedenceRuleList->precedenceRuleListLen++;
    return 0;
}

bool isTokenTypeAccepted(token *activeToken)
{
    for (size_t i = 0; i < acceptedTokenTypesLength; i++)
    {
        if (acceptedTokenTypes[i] == activeToken->tokenType)
        {
            return true;
        }
    }
    return false;
}

int setUpActiveToken(token *T)
{
    if (T == NULL)
    {
        return 99;
    }

    T->value = (string *)malloc(sizeof(string));
    if (T->value == NULL)
    {
        return 99;
    }
    strInit(T->value);

    T->position = (positionInfo *)malloc(sizeof(positionInfo));
    if (T->position == NULL)
    {
        return 99;
    }
    return 0;
}

char getPrecedence(enum tokenType topOfStackTokenType, enum tokenType currentTokenType, char *precedenceTable)
{
    unsigned int topOfStackIndex = getIndexInPrecedenceTable(topOfStackTokenType);
    unsigned int currentTokenIndex = getIndexInPrecedenceTable(currentTokenType);

    return precedenceTable[topOfStackIndex * 9 + currentTokenIndex];
}

unsigned int getIndexInPrecedenceTable(enum tokenType tokenType)
{
    // printf ("Token type: %d\n",tokenType);
    switch (tokenType)
    {
    case T_LEFT_BRACKET:
        return 0;
        break;

    case T_RIGHT_BRACKET:
        return 1;
        break;

    case T_MULTIPLICATION:
    case T_DIVISION:
        return 2;
        break;

    case T_PLUS:
    case T_MINUS:
        return 3;
        break;

    case T_LESS:
    case T_LESS_EQUAL:
    case T_GREATER:
    case T_GREATER_EQUAL:
        return 4;
        break;

    case T_EQUAL:
    case T_NOT_EQUAL:
        return 5;
        break;

    case T_NIL_OP:
        return 6;
        break;

    case T_IDENTIFIER:
    case T_INT:
    case T_DOUBLE:
    case T_STRING:
        return 7;
        break;

    case T_END:
        return 8;
        break;

    default:
        fprintf(stderr, "%s", "Error: invalid token type in expression_parser.c in getIndexInPrecedenceTable()!\n");
        return -1;
        break;
    }
}

int addLastToQueue(struct tokenQueue *tQ, token *tokenIn)
{
    struct tokenQueueElement *tQE = (struct tokenQueueElement *)malloc(sizeof(struct tokenQueueElement));
    if (tQE == NULL)
    {
        return 99;
    }

    tQE->tokenInQueue = tokenIn;
    tQE->next = NULL;
    if (tQ->first == NULL && tQ->last == NULL)
    {
        tQ->first = tQE;
        tQ->last = tQE;
    }
    else
    {
        tQ->last->next = tQE;
        tQ->last = tQE;
    }
    return 0;
}

token *getFirstFromQueue(struct tokenQueue *tQ)
{
    return tQ->first->tokenInQueue;
}

int popFirstFromQueue(struct tokenQueue *tQ)
{
    struct tokenQueueElement *tQE = tQ->first;
    if (tQ->first == tQ->last)
    {
        free(tQE);
        tQ->first = NULL;
        tQ->last = NULL;
    }
    else
    {
        tQ->first = tQ->first->next;
        free(tQE);
    }

    return 0;
}

int expressionParserStart(struct precedenceRuleList *outputPrecedenceRuleList, programState *PROGRAM_STATE)
{
    struct tokenStack *tokenStack = tokenStackInit();
    token *firstToken = (token *)malloc(sizeof(token));
    
    struct tokenQueue *tokenQueue = (struct tokenQueue *)malloc(sizeof(struct tokenQueue));

    if (firstToken == NULL || tokenStack == NULL || tokenQueue == NULL)
    {
        return 99;
    }

    firstToken->tokenType = T_END;
    tokenQueue->first = NULL;
    tokenQueue->last = NULL;

    tokenStackPush(tokenStack, firstToken);

    if (PROGRAM_STATE->isLastReadTokenValid == true)
    {
        if (!isTokenTypeAccepted(PROGRAM_STATE->lastReadToken))
        {
            fprintf(stderr, "Error: invalid token type in PROGRAM_STATE! Expression scanner did nothing\n");
            return 42;
        }
        addLastToQueue(tokenQueue, PROGRAM_STATE->lastReadToken);
        PROGRAM_STATE->isLastReadTokenValid = false;
    }

    /// activate token for scanner ///
    token *activeToken = (token *)malloc(sizeof(token));
    if (activeToken == NULL || setUpActiveToken(activeToken) != 0)
    {
        return 99;
    }

    /// READING LOOP ///
    bool reading = true;
    while (reading)
    {
        tokenClear(activeToken);
        getToken(activeToken, 1, 1);
        if (isTokenTypeAccepted(activeToken))
        {
            if (activeToken->tokenType == T_EOL)
            {
                continue;
            }
            addLastToQueue(tokenQueue, activeToken);
            activeToken = (token *)malloc(sizeof(token));
            if (activeToken == NULL || setUpActiveToken(activeToken) != 0)
            {
                return 99;
            }
        }
        else
        {
            reading = false;
            PROGRAM_STATE->isLastReadTokenValid = true;
            PROGRAM_STATE->lastReadToken = activeToken;
            activeToken = (token *)malloc(sizeof(token));
            if (activeToken == NULL || setUpActiveToken(activeToken) != 0)
            {
                return 99;
            }
            activeToken->tokenType = T_END;
            addLastToQueue(tokenQueue, activeToken);
        }
    }

    /// print all token types in queue
    struct tokenQueueElement *tQE = tokenQueue->first;
    while (tQE != NULL)
    {
        printf("Token type in queue: %d\n", tQE->tokenInQueue->tokenType);
        tQE = tQE->next;
    }

    bool running = true;

    while (running)
    {
        // print token type
        printf("\nTop of stack: %d, top of queue %d\n", whichTypeIsOnTheStack(tokenStack), getFirstFromQueue(tokenQueue)->tokenType);
        /// print tokens precedence

        switch (getPrecedence(whichTypeIsOnTheStack(tokenStack), getFirstFromQueue(tokenQueue)->tokenType, *precedenceTable))
        {
        case '<':
            printf("Push to stack\n");
            tokenStackPush(tokenStack, getFirstFromQueue(tokenQueue));
            popFirstFromQueue(tokenQueue);
            break;

        case '>':
        case '=':
            printf("Do reduction\n");
            switch (whichTypeIsOnTheStack(tokenStack))
            {
            case T_LEFT_BRACKET:
            {
                printf("E -> ( E )\n");

                struct precedenceRule *newRule = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
                newRule->description = "E -> ( E )";
                newRule->leftSide.tokenType = T_E;
                newRule->rightSideLen = 3;
                copy_Token(tokenStackGet(tokenStack, 2), &(newRule->rightSide[0]));
                copy_Token(tokenStackGet(tokenStack, 1), &(newRule->rightSide[1]));
                copy_Token(tokenStackGet(tokenStack, 0), &(newRule->rightSide[2]));
                addPrecedenceRuleToList(outputPrecedenceRuleList, newRule);
                tokenStackPop(tokenStack, 2);
                copy_Token(&(newRule->leftSide), tokenStackGet(tokenStack, 0));
                break;
            }

            case T_IDENTIFIER:
            case T_INT:
            case T_DOUBLE:
            case T_STRING:
            {
                printf("E -> i\n");

                struct precedenceRule *newRule = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
                newRule->description = "E -> i";
                newRule->leftSide.tokenType = T_E;
                newRule->rightSideLen = 1;
                newRule->rightSide = (token *)malloc(sizeof(token) * newRule->rightSideLen);
                copy_Token(tokenStackGet(tokenStack, 0), newRule->rightSide);
                addPrecedenceRuleToList(outputPrecedenceRuleList, newRule);
                copy_Token(&(newRule->leftSide), tokenStackGet(tokenStack, 0));

                break;
            }
            case T_PLUS:
            {
                printf("E -> E + E\n");

                struct precedenceRule *newRule = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
                newRule->description = "E -> E + E";
                newRule->leftSide.tokenType = T_E;
                newRule->rightSideLen = 3;
                newRule->rightSide = (token *)malloc(sizeof(token) * newRule->rightSideLen);
                copy_Token(tokenStackGet(tokenStack, 2), &(newRule->rightSide[0]));
                copy_Token(tokenStackGet(tokenStack, 1), &(newRule->rightSide[1]));
                copy_Token(tokenStackGet(tokenStack, 0), &(newRule->rightSide[2]));
                addPrecedenceRuleToList(outputPrecedenceRuleList, newRule);
                tokenStackPop(tokenStack, 2);
                copy_Token(&(newRule->leftSide), tokenStackGet(tokenStack, 0));
                break;
            }

            case T_MINUS:
            {
                printf("E -> E - E\n");
                break;
            }

            case T_MULTIPLICATION:
            {
                printf("E -> E * E\n");
                break;
            }

            case T_DIVISION:
            {
                printf("E -> E / E\n");
                break;
            }

            case T_LESS:
            {
                printf("E -> E < E\n");
                break;
            }

            case T_LESS_EQUAL:
            {
                printf("E -> E <= E\n");
                break;
            }

            case T_GREATER:
            {
                printf("E -> E > E\n");
                break;
            }

            case T_GREATER_EQUAL:
            {
                printf("E -> E >= E\n");
                break;
            }

            case T_EQUAL:
            {
                printf("E -> E == E\n");
                break;
            }

            case T_NOT_EQUAL:
            {
                printf("E -> E != E\n");
                break;
            }

            case T_NIL_OP:
            {
                printf("E -> E ?? E\n");
                break;
            }

            default:
                printf("Default state in the second switch: %d\n", whichTypeIsOnTheStack(tokenStack));
                break;
            }
            break;

        case '1':
            printf("Error: invalid expression!\n");
            return 2;
            break;

        case '0':
            printf("Parsing is done!\n");
            return 0;
            break;

        default:
            printf("Default state in the first switch: ");
            break;
        }

        // sleep(1);
    }
    return 0;
}
