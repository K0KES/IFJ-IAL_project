#include "expression_parser.h"

int parseExpression(token *tokenArr, unsigned tokenArrLenght, char thisWillBePointerOnTheTopOfTheSubDerivationTree)
{
    bool validTokenType;
    for (unsigned iToken = 0; iToken < tokenArrLenght; iToken++)
    {
        validTokenType = false;
        for(unsigned iType = 0; iType < acceptedTokenTypesLenght; iType++)
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
