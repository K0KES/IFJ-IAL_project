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
