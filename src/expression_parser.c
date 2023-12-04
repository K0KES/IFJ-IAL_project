#include "expression_parser.h"

void copyToken(token *t1, token *t2)
{
    t2->lastChar = t1->lastChar;
    t2->position->charNumber = t1->position->charNumber;
    t2->position->lineNumber = t1->position->lineNumber;
    t2->tokenExpParserType = t1->tokenExpParserType;
    t2->tokenType = t1->tokenType;
    t2->is_return_from_func = t1->is_return_from_func;
    t2->is_nullable = t1->is_nullable;
    strSetString(t2->value, strGetStr(t1->value));
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
            DEBUG_PRINTF("[Exp parser] Error: stack is too small!\n");
            return (NULL);
        }
    }
    return (tSE->tokenOnStack);
}

int isTokenTypeOperatorLike(enum tokenType tokenType)
{
    if (tokenType == T_PLUS || tokenType == T_MINUS || tokenType == T_MULTIPLICATION || tokenType == T_DIVISION || tokenType == T_LESS || tokenType == T_LESS_EQUAL || tokenType == T_GREATER || tokenType == T_GREATER_EQUAL || tokenType == T_EQUAL || tokenType == T_NOT_EQUAL || tokenType == T_NIL_OP)
    {
        return 1;
    }
    else if (tokenType == T_LEFT_BRACKET)
    {
        return 2;
    }
    else if (tokenType == T_RIGHT_BRACKET)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

int isTokenFunction(enum tokenType tokenType)
{
    if (tokenType == KW_READDOUBLE || tokenType == KW_READINT || tokenType == KW_READSTRING || tokenType == KW_INT_TO_DOUBLE || tokenType == KW_DOUBLE_TO_INT || tokenType == KW_LENGTH || tokenType == KW_SUBSTRING || tokenType == KW_ORD || tokenType == KW_CHR || tokenType == T_IDENTIFIER)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack)
{
    if (stack == NULL || stack->top == NULL || stack->top->tokenOnStack == NULL)
        DEBUG_PRINTF("[Exp parser] Segfault in whichTypeIsOnTheStack\n");

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

int checkTokensOnTopOfTheStack(struct tokenStack *stack)
{
    if (stack == NULL || stack->top == NULL)
    {
        DEBUG_PRINTF("[Exp parser] Error internal 1!\n");
        raiseError(ERR_INTERNAL);
    }
    if (tokenStackGet(stack, 0)->tokenType != T_E)
    {
        return 2;
    }
    if (tokenStackGet(stack, 2)->tokenType != T_E)
    {
        return 2;
    }

    return 0;
}

int tokenStackPush(struct tokenStack *stack, token *tokenIn)
{
    struct tokenStackElement *newElement = (struct tokenStackElement *)malloc(sizeof(struct tokenStackElement));
    token *newToken = tokenInit();
    newElement->tokenOnStack = newToken;
    if (newElement == NULL || newElement->tokenOnStack == NULL)
    {
        DEBUG_PRINTF("[Exp parser] Malloc failed in tokenStackPush\n");
        DEBUG_PRINTF("[Exp parser] Error internal 2!\n");
        raiseError(ERR_INTERNAL);
    }
    newElement->next = NULL;
    copyToken(tokenIn, newElement->tokenOnStack);
    newElement->next = stack->top;
    stack->top = newElement;

    return 0;
}

struct tokenStack *tokenStackInit()
{
    struct tokenStack *tokenStack = (struct tokenStack *)malloc(sizeof(struct tokenStack));
    if (tokenStack == NULL)
    {
        DEBUG_PRINTF("[Exp parser] Malloc failed in tokenStackInit\n");
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
            DEBUG_PRINTF("[Exp parser] Error: stack is empty!\n");
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
            DEBUG_PRINTF("[Exp parser] Error internal 3!\n");

            raiseError(ERR_INTERNAL);
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
        DEBUG_PRINTF("[Exp parser] Error internal 4!\n");

        raiseError(ERR_INTERNAL);
    }

    /*
    T->value = (string *)malloc(sizeof(string));
    if (T->value == NULL)
    {
        raiseError(ERR_INTERNAL);
    }*/
    strInit(T->value);

    T->position = (positionInfo *)malloc(sizeof(positionInfo));
    if (T->position == NULL)
    {
        DEBUG_PRINTF("[Exp parser] Error internal 5!\n");

        raiseError(ERR_INTERNAL);
    }
    return 0;
}

struct precedenceRuleList *precedenceRuleListInit()
{
    struct precedenceRuleList *precedenceRuleList = (struct precedenceRuleList *)malloc(sizeof(struct precedenceRuleList));
    if (precedenceRuleList == NULL)
    {
        return NULL;
    }
    precedenceRuleList->precedenceRuleListLen = 0;
    precedenceRuleList->precedenceRuleListAllocatedLen = 1;
    precedenceRuleList->precedenceRuleList = (struct precedenceRule **)malloc(sizeof(struct precedenceRule *) * precedenceRuleList->precedenceRuleListAllocatedLen);
    if (precedenceRuleList->precedenceRuleList == NULL)
    {
        return NULL;
    }
    return precedenceRuleList;
}

void precedenceRuleListClear(struct precedenceRuleList *precedenceRuleList)
{
    for (size_t i = 0; i < precedenceRuleList->precedenceRuleListLen; i++)
    {
        free(precedenceRuleList->precedenceRuleList[i]->rightSide);
        // free(precedenceRuleList->precedenceRuleList[i]->description);
    }
    free(precedenceRuleList->precedenceRuleList);
    free(precedenceRuleList);
    precedenceRuleList = NULL;
}

char getPrecedence(enum tokenType topOfStackTokenType, enum tokenType currentTokenType, char *precedenceTable)
{
    unsigned int topOfStackIndex = getIndexInPrecedenceTable(topOfStackTokenType);
    unsigned int currentTokenIndex = getIndexInPrecedenceTable(currentTokenType);

    return precedenceTable[topOfStackIndex * 9 + currentTokenIndex];
}

unsigned int getIndexInPrecedenceTable(enum tokenType tokenType)
{
    // DEBUG_PRINTF ("Token type: %d\n",tokenType);
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
    case KW_NIL:
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
        DEBUG_PRINTF("[Exp parser] Error internal 6!\n");

        raiseError(ERR_INTERNAL);
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

token *getLastFromQueue(struct tokenQueue *tQ)
{
    return tQ->last->tokenInQueue;
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

int expressionParserStart(programState *PS)
{

    DEBUG_PRINTF("[Exp parser] Expression parser started...\n");
    // structures init
    token *tmpDebugToken;
    struct tokenStack *tokenStack = tokenStackInit();
    token *firstToken = tokenInit();
    struct tokenQueue *tokenQueue = (struct tokenQueue *)malloc(sizeof(struct tokenQueue));

    // check malloc success
    if (firstToken == NULL || tokenStack == NULL || tokenQueue == NULL)
    {
        DEBUG_PRINTF("[Exp parser] Error internal 7!\n");

        raiseError(ERR_INTERNAL);
    }

    // add T_END token on the end of the queue
    firstToken->tokenType = T_END;
    tokenQueue->first = NULL;
    tokenQueue->last = NULL;
    int bracketsState = 0;
    tokenStackPush(tokenStack, firstToken);
    // DEBUG_PRINTF("[Exp parser] Token stack size: %d\n", PS->tokenQueue->size);

    // when you can't read everything from shared queue it prevents you from reading from scanner
    bool reading_block = false;

    // copy tokens from queue shared with scanner to internal queue
    while (PS->tokenQueue->size > 0)
    {
        if (isTokenTypeAccepted(listGetFirst(PS->tokenQueue)))
        {
            // prevents reading to much brackets
            tmpDebugToken = ((token *)listGetFirst(PS->tokenQueue));

            if (((token *)listGetFirst(PS->tokenQueue))->tokenType == T_LEFT_BRACKET)
            {
                bracketsState++;
            }
            else if (((token *)listGetFirst(PS->tokenQueue))->tokenType == T_RIGHT_BRACKET)
            {
                bracketsState--;
            }

            if (bracketsState < 0)
            {
                // token *tmpDebugToken = listGetFirst(PS->tokenQueue);
                reading_block = true;
                DEBUG_PRINTF("[Exp parser] Reading block due to just one ID in function\n");
                break;
            }
            else
            {
                addLastToQueue(tokenQueue, listPopFirst(PS->tokenQueue));
            }
        }
        else
        {
            reading_block = true;
            DEBUG_PRINTF("[Exp parser] Invalid token on shared token queue.\n");
            // DEBUG_PRINTF("[Exp parser] Special case\n");
            // DEBUG_PRINTF("[Exp parser] Error internal 8!\n");

            // raiseError(ERR_INTERNAL);
            break;
        }
    }

    /// activate token for scanner ///
    token *activeToken = tokenInit();
    enum tokenType newIdentifierType;

    /// READING LOOP ///
    bool reading = true;
    bool ignoredEOL = false;
    enum tokenType lastValidTokensTypes[] = {T_NO_TOKEN, T_NO_TOKEN};

    while (reading && !reading_block)
    {
        tokenClear(activeToken);
        // printf active token lastchar
        getToken(activeToken, activeToken->position->charNumber, activeToken->position->lineNumber);

        // prevents reading to much brackets
        if (activeToken->tokenType == T_LEFT_BRACKET)
        {
            bracketsState++;
        }
        else if (activeToken->tokenType == T_RIGHT_BRACKET)
        {
            bracketsState--;
        }

        if (bracketsState < 0)
        {
            reading = false;
            listPushBack(PS->tokenQueue, activeToken);
            break;
        }

        else if (activeToken->tokenType == T_LEFT_BRACKET && getLastFromQueue(tokenQueue) != NULL && isTokenFunction(getLastFromQueue(tokenQueue)->tokenType))
        {
            DEBUG_PRINTF("[Exp parser] Spotted function\n");
            listPushBack(PS->tokenQueue, getLastFromQueue(tokenQueue));
            token *tokenToPush = tokenInit();
            char lastCharBackup = getLastFromQueue(tokenQueue)->lastChar;
            copyToken(activeToken, tokenToPush);
            listPushBack(PS->tokenQueue, tokenToPush);

            parseFunctionCall();

            DEBUG_PRINTF("[Exp parser] Back from parser function\n");
            // DEBUG_PRINTF("[Exp parser] %d \n",listLength(PS->gen->parserStack));

            getLastFromQueue(tokenQueue)->tokenType = T_IDENTIFIER;
            getLastFromQueue(tokenQueue)->tokenExpParserType = PS->expParserReturnType;
            getLastFromQueue(tokenQueue)->lastChar = lastCharBackup;
            // getLastFromQueue(tokenQueue)->value->str = generatorPopFirstStringFromList(PS->gen->parserStack);
            // char *tmpStr = generatorPopFirstStringFromList(PS->gen->parserStack);
            strSetString(getLastFromQueue(tokenQueue)->value, generatorPopFirstStringFromList(PS->gen->parserStack));
            getLastFromQueue(tokenQueue)->is_return_from_func = true;
            // fix bracket count
            bracketsState--;
            // raiseError(ERR_INTERNAL);
            tokenToPush = tokenInit();

            copyToken(listGetFirst(PS->tokenQueue), tokenToPush);
            listPopFirst(PS->tokenQueue);

            addLastToQueue(tokenQueue, tokenToPush);

            DEBUG_PRINTF("[Exp parser] Got token from queue: %s\n", getTokenName(getLastFromQueue(tokenQueue)->tokenType));
            // token *tmpDebugToken1 = (token *)listGetFirst(PS->tokenQueue);
            lastValidTokensTypes[1] = lastValidTokensTypes[0];
            lastValidTokensTypes[0] = tokenToPush->tokenType;
        }

        else if (isTokenTypeAccepted(activeToken) && bracketsState >= 0 && !ignoredEOL)
        {
            // DEBUG_PRINTF("[Exp parser] Active token char %s and it's last char: %d\n", getTokenName(activeToken->tokenType), activeToken->lastChar);

            // manages when to stop reading tokens
            if (isTokenTypeOperatorLike(activeToken->tokenType) != 1)
            {
                if (lastValidTokensTypes[0] == T_EOL && isTokenTypeOperatorLike(lastValidTokensTypes[1]) != 1)
                {
                    // pushes last read token tto queue for parser to process it
                    token *eol_token = tokenInit();
                    eol_token->tokenType = T_EOL;
                    listPushBack(PS->tokenQueue, tokenQueue->last->tokenInQueue);
                    listPushBack(PS->tokenQueue, activeToken);

                    reading = false;
                }
            }

            if (reading)
            {
                lastValidTokensTypes[1] = lastValidTokensTypes[0];
                lastValidTokensTypes[0] = activeToken->tokenType;
                // adds token to internal queue
                addLastToQueue(tokenQueue, activeToken);
                activeToken = tokenInit();
            }
        }

        else
        {
            // deals with token that can't be handled by expression parser
            reading = false;

            // DEBUG_PRINTF("[Exp parser] Token type, that I am pushing: %s\n", getTokenName(activeToken->tokenType));

            if (tokenQueue->last->tokenInQueue->tokenType == T_EOL)
            {
                token *eol_token = tokenInit();
                eol_token->tokenType = T_EOL;
                copyToken(tokenQueue->last->tokenInQueue, eol_token);
                listPushBack(PS->tokenQueue, eol_token);
            }
            // pushes the tokent to queue for parser to process it
            listPushBack(PS->tokenQueue, activeToken);
        }
    }
    activeToken = tokenInit();

    // adding last token to list
    // if (getLastFromQueue(tokenQueue) != NULL)
    // {
    //     if (getLastFromQueue(tokenQueue)->tokenType == T_EOL)
    //     {
    //         token *eol_token = tokenInit();
    //         eol_token->tokenType = T_EOL;
    //         copyToken(getLastFromQueue(tokenQueue), eol_token);
    //         listPushBack(PS->tokenQueue, eol_token);
    //     }
    // }

    activeToken->tokenType = T_END;
    addLastToQueue(tokenQueue, activeToken);
    // exit(1);
    /// print all token types in queue
    struct tokenQueueElement *tQE = tokenQueue->first;
    while (tQE != NULL)
    {
        // DEBUG_PRINTF("[Exp parser] Token type in queue: %s\n", getTokenName(tQE->tokenInQueue->tokenType));
        tQE = tQE->next;
    }

    bool running = true;

    // check syntax around operators
    struct tokenQueueElement *tmpQueueElement = tokenQueue->first;
    enum tokenType lastTokenType = T_NO_TOKEN;
    while (tmpQueueElement != NULL)
    {
        if (isTokenTypeOperatorLike(tmpQueueElement->tokenInQueue->tokenType) == 1)
        {
            // if there is following token in queue
            if (tmpQueueElement->next != NULL)
            {
                // there is NOT space in front of operator
                if (tmpQueueElement->tokenInQueue->lastChar == 0 && lastTokenType != T_EOL)
                {
                    // next token is EOL
                    if (tmpQueueElement->next->tokenInQueue->tokenType == T_EOL)
                    {
                        DEBUG_PRINTF("[Exp parser] Error: Checking weird syntax: 1!\n");
                        raiseError(ERR_SYNTAX);
                    }
                    // next token  has space in front of him
                    if (tmpQueueElement->next->tokenInQueue->lastChar == 32)
                    {
                        DEBUG_PRINTF("[Exp parser] Error: Checking weird syntax: 2!\n");
                        raiseError(ERR_SYNTAX);
                    }
                }
                // there is space in front of operator
                else if (tmpQueueElement->tokenInQueue->lastChar == 32 || lastTokenType == T_EOL)
                {
                    if (tmpQueueElement->next->tokenInQueue->lastChar == 0 && tmpQueueElement->next->tokenInQueue->tokenType != T_EOL)
                    {
                        DEBUG_PRINTF("[Exp parser] Error: Checking weird syntax: 3!\n");
                        raiseError(ERR_SYNTAX);
                    }
                }
            }
        }
        lastTokenType = tmpQueueElement->tokenInQueue->tokenType;
        tmpQueueElement = tmpQueueElement->next;
    }

    // remove EOL from queue
    // EOL can be on the first place in the queue
    tmpQueueElement = tokenQueue->first;
    struct tokenQueueElement *queueElementToFree;
    while (tmpQueueElement != NULL)
    {
        if (tmpQueueElement->next != NULL)
        {
            if (tmpQueueElement->next->tokenInQueue->tokenType == T_EOL)
            {
                queueElementToFree = tmpQueueElement->next;
                tmpQueueElement->next = tmpQueueElement->next->next;
                if (tmpQueueElement->next->next != NULL)
                {
                    tokenFree(queueElementToFree->tokenInQueue);
                }
                free(queueElementToFree);
            }
        }
        tmpQueueElement = tmpQueueElement->next;
    }

    // check syntax errors
    tmpQueueElement = tokenQueue->first;
    lastTokenType = T_PLUS;
    while (tmpQueueElement != NULL)
    {
        if (isTokenTypeOperatorLike(tmpQueueElement->tokenInQueue->tokenType) == 1)
        {
            if (isTokenTypeOperatorLike(lastTokenType) == 1 || isTokenTypeOperatorLike(lastTokenType) == 2)
            {
                DEBUG_PRINTF("[Exp parser] Error: Checking weird syntax: 4!\n");
                raiseError(ERR_SYNTAX);
            }

            if (tmpQueueElement->next == NULL)
            {
                DEBUG_PRINTF("[Exp parser] Error: Checking weird syntax: 5!\n");
                raiseError(ERR_SYNTAX);
            }

            if (isTokenTypeOperatorLike(tmpQueueElement->next->tokenInQueue->tokenType) == 1 || isTokenTypeOperatorLike(tmpQueueElement->next->tokenInQueue->tokenType) == 3 || tmpQueueElement->next->tokenInQueue->tokenType == T_END)
            {
                DEBUG_PRINTF("[Exp parser] Error: Checking weird syntax: 6!\n");
                // raiseError(ERR_SYNTAX);
            }
        }

        lastTokenType = tmpQueueElement->tokenInQueue->tokenType;
        tmpQueueElement = tmpQueueElement->next;
    }

    // while loop which process tokens from queue
    while (running)
    {
        // print token type
        // DEBUG_PRINTF("[Exp parser] \nTop of stack: %s, top of queue %s\n", getTokenName(whichTypeIsOnTheStack(tokenStack)), getTokenName(getFirstFromQueue(tokenQueue)->tokenType));

        // ignore eols
        if (getFirstFromQueue(tokenQueue)->tokenType == T_EOL)
        {
            popFirstFromQueue(tokenQueue);
            continue;
        }

        switch (getPrecedence(whichTypeIsOnTheStack(tokenStack), getFirstFromQueue(tokenQueue)->tokenType, *precedenceTable))
        {
            // push to stack
        case '<':
            DEBUG_PRINTF("[Exp parser] Push to stack: %s\n", getTokenName(getFirstFromQueue(tokenQueue)->tokenType));
            tokenStackPush(tokenStack, getFirstFromQueue(tokenQueue));
            popFirstFromQueue(tokenQueue);
            break;

        // handling of E -> ( E ) it's kinda special case
        case '=':
        {
            // DEBUG_PRINTF("[Exp parser] Push ) to stack and do reduction\n");
            // DEBUG_PRINTF("[Exp parser] E -> ( E )\n");
            tokenStackPush(tokenStack, getFirstFromQueue(tokenQueue));
            popFirstFromQueue(tokenQueue);

            if (tokenStackGet(tokenStack, 0)->tokenType != T_RIGHT_BRACKET || tokenStackGet(tokenStack, 1)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_LEFT_BRACKET)
            {
                raiseError(ERR_SYNTAX);
            }

            copyToken(tokenStackGet(tokenStack, 1), tokenStackGet(tokenStack, 2));

            tokenStackPop(tokenStack, 2);

            break;
        }

            // do reduction
        case '>':
            // DEBUG_PRINTF("[Exp parser] Do reduction\n");
            switch (whichTypeIsOnTheStack(tokenStack))
            {

            case T_IDENTIFIER:
            {
                // DEBUG_PRINTF("[Exp parser] E -> i (identifier)\n");
                if (tokenStackGet(tokenStack, 0)->is_return_from_func == true)
                {
                    tokenStackGet(tokenStack, 0)->is_return_from_func = false;
                    tokenStackGet(tokenStack, 0)->tokenType = T_E;
                    break;
                    // raiseError(ERR_INTERNAL);
                }
                else
                {
                    newIdentifierType = symtableGetVariableType(PS->symTable, strGetStr(tokenStackGet(tokenStack, 0)->value));
                    if (newIdentifierType != T_INT && newIdentifierType != T_DOUBLE && newIdentifierType != T_STRING && newIdentifierType != KW_NIL)
                    {
                        // DEBUG_PRINTF("[Exp parser] Error: expression parser spotted potential function!\n");
                        DEBUG_PRINTF("[Exp parser] Error internal 9!\n");
                        raiseError(ERR_INTERNAL);
                    }

                    tokenStackGet(tokenStack, 0)->tokenExpParserType = newIdentifierType;
                    tokenStackGet(tokenStack, 0)->tokenType = T_E;
                    tokenStackGet(tokenStack, 0)->is_nullable = symtableGetVariableNullable(PS->symTable, strGetStr(tokenStackGet(tokenStack, 0)->value));
                    DEBUG_PRINTF("[Exp parser] E -> i (identifier)\n");
                    DEBUG_PRINTF("[Exp parser] Identifier type: %s\n", getTokenName(newIdentifierType));
                    DEBUG_PRINTF("[Exp parser] Identifier name: %s\n", strGetStr(tokenStackGet(tokenStack, 0)->value));
                    DEBUG_PRINTF("[Exp parser] Identifier is nullable: %d\n", tokenStackGet(tokenStack, 0)->is_nullable);
                    // symtableGetVariableNullable(PS->symTable, strGetStr(tokenStackGet(tokenStack, 0)->value));
                    strSetString(tokenStackGet(tokenStack, 0)->value, concatString(2, symtableGetVariablePrefix(PS->symTable, strGetStr(tokenStackGet(tokenStack, 0)->value)), strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    break;
                }
            }

            case T_INT:
            {
                // DEBUG_PRINTF("[Exp parser] E -> i (int)\n");

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));
                symtablePushCode(PS->symTable, concatString(4, "MOVE ", tempVarName, " int@", strGetStr(tokenStackGet(tokenStack, 0)->value)));

                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_INT;
                tokenStackGet(tokenStack, 0)->tokenType = T_E;

                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);

                break;
            }

            case T_STRING:
            {
                // DEBUG_PRINTF("[Exp parser] E -> i (string)\n");

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));

                char *stringWithoutQuote = strGetStr(tokenStackGet(tokenStack, 0)->value);
                stringWithoutQuote++;                                   // Remove first " from input string
                stringWithoutQuote[strlen(stringWithoutQuote) - 1] = 0; // Remove last " from string

                char *stringAssemblyValue = stringToAssemblyStringFormat(stringWithoutQuote);
                symtablePushCode(PS->symTable, concatString(4, "MOVE ", tempVarName, " string@", stringAssemblyValue));
                free(stringAssemblyValue);
                stringAssemblyValue = NULL;

                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_STRING;
                tokenStackGet(tokenStack, 0)->tokenType = T_E;

                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);

                break;
            }

            case T_DOUBLE:
            {
                // DEBUG_PRINTF("[Exp parser] E -> i (float)\n");

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));
                char floatString[100];
                sprintf(floatString, "%a", atof(strGetStr(tokenStackGet(tokenStack, 0)->value)));
                symtablePushCode(PS->symTable, concatString(4, "MOVE ", tempVarName, " float@", floatString));

                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_DOUBLE;
                tokenStackGet(tokenStack, 0)->tokenType = T_E;

                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);

                break;
            }
            case KW_NIL:
            {
                DEBUG_PRINTF("[Exp parser] E -> i (nil)\n");

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));
                symtablePushCode(PS->symTable, concatString(3, "MOVE ", tempVarName, " nil@nil"));

                tokenStackGet(tokenStack, 0)->tokenExpParserType = tokenStackGet(tokenStack, 0)->tokenType;
                tokenStackGet(tokenStack, 0)->tokenType = T_E;
                tokenStackGet(tokenStack, 0)->is_nullable = true;

                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);
                // raiseError(ERR_INTERNAL);
                break;
            }

            case T_PLUS:
            {
                DEBUG_PRINTF("[Exp parser] E -> E + E\n");

                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                // concat str
                if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_STRING && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_STRING)
                {
                    symtablePushCode(PS->symTable, concatString(6, "CONCAT ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                }

                // int + int or float + float
                else if ((tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT) || (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE))
                {
                    symtablePushCode(PS->symTable, concatString(6, "ADD ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                }
                // float + int
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 0)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "ADD ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // int + float
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "ADD ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // Error
                else
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 1\n");
                    raiseError(ERR_WRONG_TYPE);
                }
                tokenStackPop(tokenStack, 2);

                break;
            }

            case T_MINUS:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E - E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                // int + int or float + float
                if ((tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT) || (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE))
                {
                    symtablePushCode(PS->symTable, concatString(6, "SUB ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                }
                // float + int
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 0)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "SUB ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // int + float
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "SUB ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // Error
                else
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 2\n");
                    raiseError(ERR_WRONG_TYPE);
                }
                tokenStackPop(tokenStack, 2);

                break;
            }

            case T_MULTIPLICATION:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E * E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT) || (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE))
                {
                    symtablePushCode(PS->symTable, concatString(6, "MUL ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                }
                // float + int
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 0)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "MUL ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // int + float
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "MUL ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // Error
                else
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 3\n");
                    raiseError(ERR_WRONG_TYPE);
                }
                tokenStackPop(tokenStack, 2);

                break;
            }

            case T_DIVISION:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E / E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT) || (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE))
                {
                    symtablePushCode(PS->symTable, concatString(6, "DIV ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                }
                // float + int
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_INT && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_DOUBLE)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 0)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "IDIV ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // int + float
                else if (tokenStackGet(tokenStack, 0)->tokenExpParserType == T_DOUBLE && tokenStackGet(tokenStack, 2)->tokenExpParserType == T_INT)
                {
                    symtablePushCode(PS->symTable, concatString(4, "INT2FLOAT ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value)));
                    symtablePushCode(PS->symTable, concatString(6, "IDIV ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = T_DOUBLE;
                }
                // Error
                else
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 4\n");
                    raiseError(ERR_WRONG_TYPE);
                }
                tokenStackPop(tokenStack, 2);
                break;
            }

            case T_LESS:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E < E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 2)->tokenExpParserType != tokenStackGet(tokenStack, 0)->tokenExpParserType))
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 5\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));

                symtablePushCode(PS->symTable, concatString(6, "LT ", tempVarName, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                tokenStackPop(tokenStack, 2);
                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_BOOL;
                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);

                break;
            }

            case T_LESS_EQUAL:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E <= E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 2)->tokenExpParserType != tokenStackGet(tokenStack, 0)->tokenExpParserType))
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 6\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                char *tempGeneratedName_0 = generatorGenerateTempVarName(PS->gen);
                char *tempVarName_0 = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName_0), tempGeneratedName_0);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName_0));
                symtablePushCode(PS->symTable, concatString(6, "LT ", tempVarName_0, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));

                char *tempGeneratedName_1 = generatorGenerateTempVarName(PS->gen);
                char *tempVarName_1 = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName_1), tempGeneratedName_1);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName_1));
                symtablePushCode(PS->symTable, concatString(6, "EQ ", tempVarName_1, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));

                symtablePushCode(PS->symTable, concatString(6, "OR ", tempVarName_0, " ", tempVarName_0, " ", tempVarName_1));
                tokenStackPop(tokenStack, 2);
                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_BOOL;
                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName_0);
                break;
            }

            case T_GREATER:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E > E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 2)->tokenExpParserType != tokenStackGet(tokenStack, 0)->tokenExpParserType))
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 7\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));

                symtablePushCode(PS->symTable, concatString(6, "GT ", tempVarName, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                tokenStackPop(tokenStack, 2);
                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_BOOL;
                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);
                break;
            }

            case T_GREATER_EQUAL:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E >= E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 2)->tokenExpParserType != tokenStackGet(tokenStack, 0)->tokenExpParserType))
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 8\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                char *tempGeneratedName_0 = generatorGenerateTempVarName(PS->gen);
                char *tempVarName_0 = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName_0), tempGeneratedName_0);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName_0));
                symtablePushCode(PS->symTable, concatString(6, "GT ", tempVarName_0, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));

                char *tempGeneratedName_1 = generatorGenerateTempVarName(PS->gen);
                char *tempVarName_1 = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName_1), tempGeneratedName_1);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName_1));
                symtablePushCode(PS->symTable, concatString(6, "EQ ", tempVarName_1, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));

                symtablePushCode(PS->symTable, concatString(6, "OR ", tempVarName_0, " ", tempVarName_0, " ", tempVarName_1));
                tokenStackPop(tokenStack, 2);
                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_BOOL;
                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName_0);
                break;
            }

            case T_EQUAL:
            {
                // DEBUG_PRINTF("[Exp parser] E -> E == E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 2)->tokenExpParserType != tokenStackGet(tokenStack, 0)->tokenExpParserType))
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 9\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));

                symtablePushCode(PS->symTable, concatString(6, "EQ ", tempVarName, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                tokenStackPop(tokenStack, 2);
                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_BOOL;
                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);
                break;
            }

            case T_NOT_EQUAL:
            {
                // i DEBUG_PRINTF("[Exp parser] E -> E != E\n");
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }
                // check if any of the operand is nullable
                if (tokenStackGet(tokenStack, 0)->is_nullable == true || tokenStackGet(tokenStack, 2)->is_nullable == true)
                {
                    DEBUG_PRINTF("[Exp parser] Error: Can't add nullable variable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                if ((tokenStackGet(tokenStack, 2)->tokenExpParserType != tokenStackGet(tokenStack, 0)->tokenExpParserType))
                {
                    DEBUG_PRINTF("[Exp parser] Error: Wrong type in expression 10\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                char *tempGeneratedName = generatorGenerateTempVarName(PS->gen);
                char *tempVarName = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedName), tempGeneratedName);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarName));

                symtablePushCode(PS->symTable, concatString(6, "LT ", tempVarName, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));
                symtablePushCode(PS->symTable, concatString(4, "NOT ", tempVarName, " ", tempVarName));
                tokenStackPop(tokenStack, 2);
                tokenStackGet(tokenStack, 0)->tokenExpParserType = T_BOOL;
                strSetString(tokenStackGet(tokenStack, 0)->value, tempVarName);
                break;
            }

            case T_NIL_OP:
            {

                DEBUG_PRINTF("[Exp parser] --------------------------------------------------------\n");
                // DEBUG_PRINTF("[Exp parser]  E -> E ?? E\n");
                DEBUG_PRINTF("[Exp parser]  %s ?? %s \n", getTokenName(tokenStackGet(tokenStack, 2)->tokenType), getTokenName(tokenStackGet(tokenStack, 0)->tokenType));
                DEBUG_PRINTF("[Exp parser]  %s ?? %s \n", getTokenName(tokenStackGet(tokenStack, 2)->tokenExpParserType), getTokenName(tokenStackGet(tokenStack, 0)->tokenExpParserType));
                DEBUG_PRINTF("[Exp parser]  %d ?? %d \n", tokenStackGet(tokenStack, 2)->is_nullable, tokenStackGet(tokenStack, 0)->is_nullable);
                DEBUG_PRINTF("[Exp parser] --------------------------------------------------------\n");

                // check if the syntax of the operation is right
                if (tokenStackGet(tokenStack, 0)->tokenType != T_E || tokenStackGet(tokenStack, 1)->tokenType != T_NIL_OP || tokenStackGet(tokenStack, 2)->tokenType != T_E)
                {
                    DEBUG_PRINTF("[Exp parser] Syntax error missing operand!\n");
                    raiseError(ERR_SYNTAX);
                }

                // check if first operator is nullable
                if (tokenStackGet(tokenStack, 2)->is_nullable != true)
                {
                    DEBUG_PRINTF("[Exp parser] Fist operand in ?? is not nullable!\n");
                    raiseError(ERR_WRONG_TYPE);
                }

                // check if are both operands same type of if any of them is nil
                if (tokenStackGet(tokenStack, 0)->tokenExpParserType != tokenStackGet(tokenStack, 2)->tokenExpParserType && tokenStackGet(tokenStack, 0)->tokenExpParserType != KW_NIL && tokenStackGet(tokenStack, 2)->tokenExpParserType != KW_NIL)
                {
                    DEBUG_PRINTF("[Exp parser] Not matching operand types in ?? \n");
                    raiseError(ERR_WRONG_TYPE);
                }

                // creates temporary nil variable
                char *tempGeneratedNameNil = generatorGenerateTempVarName(PS->gen);
                char *tempVarNameNil = concatString(2, symtableGetVariablePrefix(PS->symTable, tempGeneratedNameNil), tempGeneratedNameNil);
                symtablePushCode(PS->symTable, concatString(2, "DEFVAR ", tempVarNameNil));
                symtablePushCode(PS->symTable, concatString(3, "MOVE ", tempVarNameNil, " nil@nil"));

                // creates labels
                char *ifNil = generatorGenerateTempVarName(PS->gen);
                char *ifNotNil = generatorGenerateTempVarName(PS->gen);

                // the decision if jump
                symtablePushCode(PS->symTable, concatString(6, "JUMPIFNEQ ", ifNotNil, " ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", tempVarNameNil));

                // is nil so it should be swapped
                symtablePushCode(PS->symTable, concatString(2, "LABEL ", ifNil));
                symtablePushCode(PS->symTable, concatString(4, "MOVE ", strGetStr(tokenStackGet(tokenStack, 2)->value), " ", strGetStr(tokenStackGet(tokenStack, 0)->value)));

                symtablePushCode(PS->symTable, concatString(2, "LABEL ", ifNotNil));

                // set right return type
                if (tokenStackGet(tokenStack, 2)->tokenExpParserType == KW_NIL)
                {
                    tokenStackGet(tokenStack, 2)->tokenExpParserType = tokenStackGet(tokenStack, 0)->tokenExpParserType;
                }
                tokenStackGet(tokenStack, 2)->is_nullable = true;
                tokenStackPop(tokenStack, 2);
                break;
            }

            default:
                // it's unexpected behavior so I say it is an error
                DEBUG_PRINTF("[Exp parser] Default state in the second switch: %s\n", getTokenName(whichTypeIsOnTheStack(tokenStack)));
                raiseError(ERR_SYNTAX);
                break;
            }
            break;

        case '1':
            // tow tokens that should not follow each other follow each other
            // function in expression
            // if (whichTypeIsOnTheStack(tokenStack) == T_IDENTIFIER && tokenQueue->first->tokenInQueue->tokenType == T_LEFT_BRACKET)
            // {
            //     DEBUG_PRINTF("[Exp parser] Function in expression!\n");
            //     listPushBack(PS->tokenQueue, tokenStackGet(tokenStack, 0));
            //     tokenStackPop(tokenStack, 1);
            //     listPushBack(PS->tokenQueue, getFirstFromQueue(tokenQueue));
            //     popFirstFromQueue(tokenQueue);
            //     bracketsState = 1;
            //     token* copyToken;
            //     while (bracketsState != 0 && getFirstFromQueue != NULL)
            //     {
            //         copyToken = getFirstFromQueue(tokenQueue);
            //         popFirstFromQueue(tokenQueue);
            //         if (copyToken->tokenType == T_LEFT_BRACKET)
            //         {
            //             bracketsState++;
            //         }
            //         else if (copyToken->tokenType == T_RIGHT_BRACKET)
            //         {
            //             bracketsState--;
            //         }
            //         tokenStackPush(tokenStack, copyToken);
            //     }
            //     parseFunctionCall();
            //     generatorPopFirstStringFromList(PS->gen->parserStack);
            //     PS->expParserReturnType;
            // }

            DEBUG_PRINTF("[Exp parser] Error: Two tokens that should not follow each other!\n");
            raiseError(ERR_SYNTAX);
            tokenStackClear(tokenStack);
            free(tokenQueue);
            free(activeToken);
            free(firstToken);
            return 0;
            break;

        case '0':;
            // parsing vas done successfully
            // DEBUG_PRINTF("[Exp parser] Parsing is done!\n");
            if (PS->tokenQueue->size > 0)
            {
                tmpDebugToken = getFirstFromQueue(PS->tokenQueue);
            }

            char *returnAdr = malloc(sizeof(char) * strGetLength(tokenStackGet(tokenStack, 0)->value));
            strcpy(returnAdr, strGetStr(tokenStackGet(tokenStack, 0)->value));
            generatorPushStringFirstToList(PS->gen->parserStack, returnAdr);
            DEBUG_PRINTF("[Exp Parser] return type: %s\n", getTokenName(tokenStackGet(tokenStack, 0)->tokenExpParserType));
            PS->expParserReturnType = tokenStackGet(tokenStack, 0)->tokenExpParserType;

            // clear memory
            tokenFree(activeToken);
            tokenFree(firstToken);
            tokenStackClear(tokenStack);
            free(tokenQueue);
            tokenQueue = NULL;
            // symtablePushCode(PS->symTable, "#Expression parser ended!\n");

            return 1;
            break;

        default:
            // unexpected state so I raise an error
            DEBUG_PRINTF("[Exp parser] Default state in the first switch: ");
            raiseError(ERR_SYNTAX);
            break;
        }

        // sleep(1);
    }
    DEBUG_PRINTF("[Exp parser] Error: Expression parser ended unexpectedly!\n");
    raiseError(ERR_INTERNAL);
    tokenStackClear(tokenStack);
    free(tokenQueue);
    free(activeToken);
    free(firstToken);
    tokenQueue = NULL;
    activeToken = NULL;
    firstToken = NULL;

    return 0;
}
