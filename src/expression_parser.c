#include "expression_parser.h"

int parseExpression(token *tokenArr, unsigned tokenArrLenght, char thisWillBePointerOnTheTopOfTheSubDerivationTree)
{
    // Check if tokenArr is not empty
    if (tokenArrLenght == 0)
    {
        return 1;
    }

    // Check if all tokens are supported
    bool validTokenType;
    for (unsigned iToken = 0; iToken < tokenArrLenght; iToken++)
    {
        validTokenType = false;
        for (unsigned iType = 0; iType < acceptedTokenTypesLenght; iType++)
        {

            if (tokenArr[iToken].tokenType == acceptedTokenTypes[iType])
            {
                validTokenType = true;
                break;
            }
        }
        if (!validTokenType)
        {
            return 1;
        }
    }

    return 0;
}

char getPrecedence(token topOfStack, token currentToken, char *precedenceTable)
{
    unsigned int topOfStackIndex = getIndexInPrecedenceTable(topOfStack.tokenType);
    unsigned int currentTokenIndex = getIndexInPrecedenceTable(currentToken.tokenType);

    return precedenceTable[topOfStackIndex * 9 + currentTokenIndex];
}



unsigned int getIndexInPrecedenceTable(enum tokenType tokenType)
{
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
        return 7;
        break;

    case T_EOF:
        return 8;
        break;

    default:
        return -1;
        break;
    }
}
