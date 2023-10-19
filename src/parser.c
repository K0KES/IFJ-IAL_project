#include "parser.h"
#include "scanner.h"

FILE *f;

// int main(int argc, char const *argv[]){
//     token *activeToken = (token*)malloc(sizeof(token));
//     strInit(activeToken->value);
//     activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
//     activeToken->position->charNumber = 0;
//     activeToken->position->lineNumber = 1;

//     if (argc == 1) {
//         printf("Neni zadan vstupni soubor\n");
//         return 0;
//     }
//     if ((f = fopen(argv[1], "r")) == NULL) {
//         printf("Soubor se nepodarilo otevrit\n");
//         return 1;
//     } 

//     getNextToken(activeToken);
//     //printf(activeToken->value);
//     if(start(activeToken)){
//         printf("Success");
//     }
//     else{
//         printf("Error");
//     }
// }

int getNextToken(token *activeToken){
    int getTokenErr = getToken(activeToken, activeToken->position->charNumber, activeToken->position->lineNumber, f);
    
    return getTokenErr;
}

bool start(token *activeToken){
    bool startStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_FUNC:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
        case T_EOF:
            // 1) <start> -> <code>
            startStatus = code(activeToken);
        case T_END:
            // 2) <start> -> EPS
            getNextToken(activeToken);  //TO DO maybe verify if non token is after END
            startStatus = true;
        default:  
            return false;
    }
    return startStatus;
}

bool code(token *activeToken){
    bool codeStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
            // 4) <code> -> <statement> <code>
            codeStatus = statement(activeToken) && code(activeToken);
        case KW_FUNC:
            // 3) <code> -> <definition> <eol> <code>
            codeStatus = definition(activeToken) && eol(activeToken) && code(activeToken);
        case T_EOF:
            // 5) <code> -> EOF
            getNextToken(activeToken);  //TO DO maybe verify if non token is after EOF
            codeStatus = true;
        default:
            return false;
        
    }
    return codeStatus;
}

bool eol(token *activeToken){
    bool eolStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_FUNC:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
        case T_EOF:
            // 7) <eol> -> EPS
            getNextToken(activeToken);  //TO DO maybe verify if non token is after EOF
            eolStatus = true;
        case T_EOL:
            // 6) <eol> -> EOL <eol>
            getNextToken(activeToken);
            eolStatus = eol(activeToken);
        default:
            return false;
    }
    return eolStatus;
}

bool type(token *activeToken){
    bool typeStatus = false;

    //TO DO nevím jestli jsou to ty správné tokeny na typy
    switch(activeToken->tokenType) {
        case KW_INT:
            // 8) <type> -> int
            getNextToken(activeToken);
            typeStatus = true;
        case KW_DOUBLE:
            // 9) <type> -> double
            getNextToken(activeToken);
            typeStatus = true;
        case KW_STRING:
            // 10) <type> -> string
            getNextToken(activeToken);
            typeStatus = true;
        default: 
            return false;
    }
    return typeStatus;
}

bool definition(token *activeToken){
    bool definitionStatus = false;

    switch(activeToken->tokenType) {
        case KW_FUNC:
            // 11) <definition> -> func ID (<functionParams>) <funcDefMid>

            // verification of: func ID 
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){ 
                return false;
            }

            // verification of: func ID(
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){ 
                return false;
            }

            // verification of: func ID(<functionParams>
            getNextToken(activeToken);
            definitionStatus = functionParams(activeToken);

            // verification of: func ID(<functionParams>)
            if (activeToken->tokenType != T_RIGHT_BRACKET){ 
                return false;
            }

            // verification of: func ID(<functionParams>) <funcDefMid>
            getNextToken(activeToken);
            definitionStatus = definitionStatus && funcDefMid(activeToken);
        default:
            return false;
    }
    return definitionStatus;
}

bool funcDefMid(token *activeToken){
    bool funcDefMidStatus = false;

    switch(activeToken->tokenType) {
        case T_LEFT_CURLY_BRACKET:
            // 12) <funcDefMid> -> {<statements>}
            getNextToken(activeToken);
            funcDefMidStatus = statements(activeToken);

            // verification of: right curly bracket
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                return false;
            }
            getNextToken(activeToken);
        case T_ARROW:
            // 13) <funcDefMid> -> -> <type> {<statements>}
            getNextToken(activeToken);
            funcDefMidStatus = type(activeToken);

            // verification of: left curly bracket
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                return false;
            }

            getNextToken(activeToken);
            funcDefMidStatus = funcDefMidStatus && statements(activeToken);

            // verification of: right curly bracket
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                return false;
            }
            getNextToken(activeToken);
        default:
            return false;
    }
    return funcDefMidStatus;
}

bool functionParams(token *activeToken){
    bool functionParamsStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_UNDERSCORE:
            // 15) <functionParams> -> <functionParam> <functionParamsN>
            functionParamsStatus = functionParam(activeToken) && functionParamsN(activeToken);
        case T_RIGHT_BRACKET:
            // 14) <functionParams> -> EPS
            getNextToken(activeToken);
            functionParamsStatus = true;
        default:
            return false;
    }
    return functionParamsStatus;
}

bool functionParamsN(token *activeToken){
    bool functionParamsNStatus = false;

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
            // 17) <functionParamsN> -> EPS
            getNextToken(activeToken);
            functionParamsNStatus = true;
        case T_COMMA:
            // 16) <functionParamsN> -> , <functionParam> <functionParamsN>
            getNextToken(activeToken);
            functionParamsNStatus = functionParam(activeToken) && functionParamsN(activeToken);
        default:
            return false;
    }
    return functionParamsNStatus;
}

bool functionParam(token *activeToken){
    bool functionParamStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 19) <functionParam> -> ID ID : <type>

            // verification of: second ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                return false;
            }

            // verification of: colon
            getNextToken(activeToken);
            if (activeToken->tokenType != T_COLON){
                return false;
            }

            getNextToken(activeToken);
            functionParamStatus = type(activeToken);
        case KW_UNDERSCORE:
            // 18) <functionParam> -> _ ID : <type>
            
            // verification of: ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                return false;
            }

            // verification of: colon
            getNextToken(activeToken);
            if (activeToken->tokenType != T_COLON){
                return false;
            }

            getNextToken(activeToken);
            functionParamStatus = type(activeToken);
        default:
            return false;
    }
    return functionParamStatus;
}

bool statements(token *activeToken){
    bool statementsStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
            // 20) <statements> -> <statement> <eol> <statements>
            statementsStatus = statement(activeToken) && eol(activeToken) && statements(activeToken);
        case T_RIGHT_CURLY_BRACKET:
            // 21) <statements> -> EPS
            getNextToken(activeToken);
            statementsStatus = true;
        default:
            return false;
    }
    return statementsStatus;
}

bool statement(token *activeToken){
    bool statementStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 26) <statement> -> ID <callOrAssign>
            getNextToken(activeToken);
            statementStatus = callOrAssign(activeToken);
        case KW_RETURN:
            // 25) <statement> -> return <returnExpression>
            getNextToken(activeToken);
            statementStatus = returnExpression(activeToken);
        case KW_IF:
            // 23) <statement> -> if <expression> {<statements>} else {<statements>}
            getNextToken(activeToken);
            statementStatus = expression(activeToken);  //TO DO jak to bude s expression (má potom být další getNextToken())

            // verification of: {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: }
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                return false;
            }

            // verification of: else
            getNextToken(activeToken);
            if (activeToken->tokenType != KW_ELSE){
                return false;
            }

            // verification of: {
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: }
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                return false;
            }
            getNextToken(activeToken);
        case KW_WHILE:
            // 24) <statement> -> while <expression> {<statements>}
            getNextToken(activeToken);
            statementStatus = expression(activeToken);

            // verification of: {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: }
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                return false;
            }
            getNextToken(activeToken);
        case KW_LET:
        case KW_VAR:
            // 22) <statement> -> <varDec>
            statementStatus = varDec(activeToken);
        default:
            return false;  
    }
    return statementStatus;
}

bool callOrAssign(token *activeToken){
    bool callOrAssignStatus = false;

    switch(activeToken->tokenType) {
        case T_LEFT_BRACKET:
            // 28) <callOrAssign> -> (<arguments>)
            getNextToken(activeToken);
            callOrAssignStatus = arguments(activeToken);

            // verification of: )
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                return false;
            }
            getNextToken(activeToken);
        case T_ASSIGNMENT:
            // 27) <callOrAssign> -> = <expression>
            getNextToken(activeToken);
            callOrAssignStatus = expression(activeToken);
        default:
            return false;
    }
    return callOrAssignStatus;
}

bool varDec(token *activeToken){
    bool varDecStatus = false;

    switch(activeToken->tokenType) {
        case KW_LET:
            // 29) <varDec> -> let ID <varDecMid>
            
            // verification of: ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                return false;
            }

            getNextToken(activeToken);
            varDecStatus = varDecMid(activeToken);
        case KW_VAR:
            // 30) <varDec> -> var ID <varDecMid>
            
            // verification of: ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                return false;
            }

            getNextToken(activeToken);
            varDecStatus = varDecMid(activeToken);
        default:
            return false;
    }
    return varDecStatus;
}

bool varDecMid(token *activeToken){
    bool varDecMidStatus = false;

    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
            // 32) <varDecMid> -> = <expression>
            getNextToken(activeToken);
            varDecMidStatus = expression(activeToken);
        case T_COLON:
            // 31) <varDecMid> -> : <type> <varDef>
            getNextToken(activeToken);
            varDecMidStatus = type(activeToken) && varDef(activeToken);
        default:
            return false;
    }
    return varDecMidStatus;
}

bool varDef(token *activeToken){
    bool varDefStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
        case KW_FUNC:
        case T_RIGHT_CURLY_BRACKET:
        case T_EOF:
        case T_EOL:
            // 33) <varDef> -> EPS
            getNextToken(activeToken);
            varDefStatus = true;
        case T_ASSIGNMENT:
            // 34) <varDef> -> = <expression>
            getNextToken(activeToken);
            varDefStatus = expression(activeToken);
        default:
            return false; 
    }
    return varDefStatus;
}

bool returnExpression(token *activeToken){
    bool returnExpressionStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
        case KW_FUNC:
        case T_RIGHT_CURLY_BRACKET:
        case T_EOF:
        case T_EOL:
            // 36) <returnExpression> -> EPS
            getNextToken(activeToken);
            returnExpressionStatus = true;
        /*case <expression>:  //TO DO co s tím
            // 35) <returnExpression> -> <expression>
            returnExpressionStatus = expression(activeToken);
        */
        default:
            return false;
    }
    return returnExpressionStatus;
}

bool arguments(token *activeToken){
    bool argumentsStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 38) <arguments> -> <argument> <argumentsN>
            argumentsStatus = argument(activeToken) && argumentsN(activeToken);
        case T_RIGHT_BRACKET:
            // 37) <arguments> -> EPS
            getNextToken(activeToken);
            argumentsStatus = true;
        default:
            return false;
    }
    return argumentsStatus;
}

bool argumentsN(token *activeToken){
    bool argumentsNStatus = false;

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
            // 40) <argumentsN> -> EPS
            getNextToken(activeToken);
            argumentsNStatus = true;
        case T_COMMA:
            // 39) <argumentsN> -> , <argument> <argumentsN>
            getNextToken(activeToken);
            argumentsNStatus = argument(activeToken) && argumentsN(activeToken);
        default:
            return false;
    }
    return argumentsNStatus;
}

bool argument(token *activeToken){
    bool argumentStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 42) <argument> -> ID <argWithName>
            getNextToken(activeToken);
            argumentStatus = argWithName(activeToken);
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 41) <argument> -> <dataType>
            argumentStatus = dataType(activeToken);
        case T_RIGHT_BRACKET:
        case T_COMMA:
            // 43) <argument> -> EPS
            getNextToken(activeToken);
            argumentStatus = true;
        default:
            return false;
    }
    return argumentStatus;
}

bool argWithName(token *activeToken){
    bool argWithNameStatus = false;

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
        case T_COMMA:
            // 45) <argWithName> -> EPS
            getNextToken(activeToken);
            argWithNameStatus = true;
        case T_COLON:
            // 44) <argWithName> -> : <argumentType>
            getNextToken(activeToken);
            argWithNameStatus = argumentType(activeToken);
        default:
            return false;
    }
    return argWithNameStatus;
}

bool argumentType(token *activeToken){
    bool argumentTypeStatus = false;

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 46) <argumentType> -> ID
            getNextToken(activeToken);
            argumentTypeStatus = true;
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 47) <argumentType> -> <dataType>
            argumentTypeStatus = dataType(activeToken);
        default:
            return false;
    }
    return argumentTypeStatus;
}

bool dataType(token *activeToken){
    bool dataTypeStatus = false;

    switch(activeToken->tokenType) {
        case T_INT:
            // 48) <dataType> -> IntData
            getNextToken(activeToken);
            dataTypeStatus = true;
        case T_DOUBLE:
            // 50) <dataType> -> DoubleData
            getNextToken(activeToken);
            dataTypeStatus = true;
        case T_STRING:
            // 49) <dataType> -> StringData
            getNextToken(activeToken);
            dataTypeStatus = true;
        default:
            return false;
    }
    return dataTypeStatus;
}

bool expression(token *activeToken){
    getNextToken(activeToken);
    return true;
}