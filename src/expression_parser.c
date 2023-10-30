#include "expression_parser.h"

void copy_Token(token *t1, token *t2)
{
    t2->lastChar = t1->lastChar;
    t2->position = t1->position;
    t2->tokenType = t1->tokenType;
    t2->value = t2->value;
}

token* tokenStackGet(struct tokenStack *stack, unsigned location)
{
    struct tokenStackElement tSE = *stack->top;
    for (unsigned i = 0; i < location; i++)
    {
        if (tSE.next != NULL)
        {
            tSE = *tSE.next;
        }
        else
        {
            printf("Error: stack is too small!\n");
            return (NULL);
        }
    }
    return (tSE.tokenOnStack);
}

enum tokenType whichTypeIsOnTheStack(struct tokenStack *stack)
{
    if (stack == NULL ||stack->top == NULL || stack->top->tokenOnStack == NULL)
        printf("Segfault in whichTypeIsOnTheStack\n");

    if (stack->top->tokenOnStack->tokenType != T_E)
        return stack->top->tokenOnStack->tokenType;
    else
        return stack->top->next->tokenOnStack->tokenType;
}


void tokenStackClear(struct tokenStack *stack)
{
    struct tokenStackElement *currentElement = stack->top;
    while (currentElement != NULL)
    {
        struct tokenStackElement *nextElement = currentElement->next;
        free (currentElement->tokenOnStack);
        free(currentElement);
        currentElement = nextElement;
    }
    free(stack);
}

int tokenStackPush(struct tokenStack *stack, token *tokenIn)
{
    struct tokenStackElement *newElement = (struct tokenStackElement *) malloc(sizeof(struct tokenStackElement));
    token *newToken = (token *) malloc(sizeof(token));
    newElement->tokenOnStack = newToken;
    if (newElement == NULL || newElement->tokenOnStack == NULL)
    {
        printf("Malloc failed in tokenStackPush\n");
        return 99;
    }
    copy_Token(tokenIn,newElement->tokenOnStack);
    newElement->next = stack->top;
    stack->top = newElement;

    return 0;
}

int tokenStackPop(struct tokenStack *stack, unsigned numberOfPops)
{
    for (unsigned i = 0; i < numberOfPops; i++){
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

int addPrecedenceRuleToList(struct precedenceRuleList *precedenceRuleList,struct precedenceRule *precedenceRule)
{
    if (precedenceRuleList->precedenceRuleListLen == precedenceRuleList->precedenceRuleListAllocatedLen)
    {
        precedenceRuleList->precedenceRuleListAllocatedLen *= 2;
        precedenceRuleList->precedenceRuleList = (struct precedenceRule **)realloc(precedenceRuleList->precedenceRuleList,precedenceRuleList->precedenceRuleListAllocatedLen * sizeof(struct precedenceRule *));
        if (precedenceRuleList->precedenceRuleList == NULL)
        {
            return 99;
        }
    }
    precedenceRuleList->precedenceRuleList[precedenceRuleList->precedenceRuleListLen] = precedenceRule;
    precedenceRuleList->precedenceRuleListLen++;
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


int expressionParserStart(struct precedenceRuleList *outputPrecedenceRuleList, programState *PROGRAM_STATE)
{
    struct tokenStack *tokenStack = (struct tokenStack *)malloc(sizeof(tokenStack));
    token *firstToken = (token *)malloc(sizeof(token));
    firstToken->tokenType = T_END;
    if (firstToken == NULL || tokenStack == NULL)
    {
        return 99;
    }
    tokenStackPush(tokenStack, firstToken);
    
    if (PROGRAM_STATE->isLastReadTokenValid == true){
        tokenStackPush(tokenStack, PROGRAM_STATE->lastReadToken);
        PROGRAM_STATE->isLastReadTokenValid = false;
    }
    
    token *activeToken = (token *)malloc(sizeof(token));
    if(activeToken == NULL){
        return 99;
    }

    activeToken->value = (string*)malloc(sizeof(string));
    if(activeToken->value == NULL){
        return 99;
    }
    strInit(activeToken->value);

    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    if(activeToken->position == NULL){
        return 99;
    }
    bool amIReading = true;

    getToken(activeToken,1,1);
    while (amIReading)
    {
        //print token type
        printf ("Token type that has been read: %d\n",activeToken->tokenType);
        amIReading = false;

        // check if am I accepting this type of token
        for (size_t i = 0; i < acceptedTokenTypesLength; i++)
        {
            if (acceptedTokenTypes[i] == activeToken->tokenType)
            {
                amIReading = true;
                break;
            }
        }

        if (amIReading)
        {
            if (activeToken->tokenType == T_EOL)
            {
                getToken(activeToken,1,1);
                continue;
            }
            printf("Top of stack: %d\n",whichTypeIsOnTheStack(tokenStack));
            
            switch (getPrecedence(whichTypeIsOnTheStack(tokenStack),activeToken->tokenType,*precedenceTable))
            {
            case '<':
                printf("Push to stack\n");
                tokenStackPush(tokenStack, activeToken);
                getToken(activeToken,1,1);
                break;
            
            case '>':
            case '=':
                printf("Do reduction\n");
                switch (whichTypeIsOnTheStack(tokenStack))
                {
                case T_RIGHT_BRACKET:
                    printf("E -> ( E )\n");
                    break;


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
                    newRule->rightSide = (token *)malloc(sizeof(token));
                    copy_Token(tokenStack->top->tokenOnStack,newRule->rightSide);
                    addPrecedenceRuleToList(outputPrecedenceRuleList,newRule);
                    copy_Token(&(newRule->leftSide),tokenStackGet(tokenStack,0));
                    

                    // int a = 5 / 0;
                    // (outputPrecedenceRuleList->precedenceRuleList[outputPrecedenceRuleList->precedenceRuleListLen]) = (struct precedenceRule *)malloc(sizeof(struct precedenceRule));
                    break;
                }
                case T_PLUS:
                    printf("E -> E + E\n");
                    break;

                case T_MINUS:
                    printf("E -> E - E\n");
                    break;
                
                case T_MULTIPLICATION:
                    printf("E -> E * E\n");
                    break;

                case T_DIVISION:
                    printf("E -> E / E\n");
                    break;

                case T_LESS:
                    printf("E -> E < E\n");
                    break;

                case T_LESS_EQUAL:
                    printf("E -> E <= E\n");
                    break;

                case T_GREATER: 
                    printf("E -> E > E\n");
                    break;
                
                case T_GREATER_EQUAL:
                    printf("E -> E >= E\n");
                    break;

                case T_EQUAL:
                    printf("E -> E == E\n");
                    break;

                case T_NOT_EQUAL:
                    printf("E -> E != E\n");
                    break;

                case T_NIL_OP:
                    printf("E -> E ?? E\n");
                    break;


                default:
                    printf("Default state in the second switch: %d\n",whichTypeIsOnTheStack(tokenStack));
                    break;
                }
                break;

            case '1':
                printf("Error: invalid expression!\n");
                return 2;
                break;

            case '0':
                printf("Parsing is done!");
                break;

            default:
                printf("Default state in the first switch: ");
                break;
            }
        }
        else
        {
            PROGRAM_STATE->isLastReadTokenValid = true;
            PROGRAM_STATE->lastReadToken = activeToken;
        }
        sleep (1);
    }
    return 0;
}

