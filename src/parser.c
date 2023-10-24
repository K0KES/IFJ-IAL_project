#include "parser.h"

int parse(token *activeToken){
    getNextToken(activeToken);
    if(start(activeToken)){
        printf("Success\n");
        return 0;
    }
    else{
        printf("Error\n");
        return 1;
    }
}

int getNextToken(token *activeToken){
    int getTokenErr = getToken(activeToken, activeToken->position->charNumber, activeToken->position->lineNumber);

    printf("Got next token: %s\n",getTokenName(activeToken->tokenType));
    
    return getTokenErr;
}

bool start(token *activeToken){
    bool startStatus = false;
    printf("Start token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function start()...\n");

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
            break;
        case T_END:
            // 2) <start> -> EPS
            getNextToken(activeToken);  //TO DO maybe verify if non token is after END
            startStatus = true;
            break;
        default:  
            printf("Leaving function start()...\n");
            return false;
    }
    printf("Leaving function start()...\n");
    return startStatus;
}

bool code(token *activeToken){
    bool codeStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function code()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
            // 4) <code> -> <statement> EOL/EOF <code>
            codeStatus = statement(activeToken);

            // verification of: EOF
            if (activeToken->tokenType == T_EOF){ 
                printf("Leaving function code()...\n");
                return codeStatus;
            }

            // verification of: EOL
            if (activeToken->tokenType != T_EOL){ 
                printf("Leaving function code()...\n");
                return false;
            }

            getNextToken(activeToken);
            codeStatus = codeStatus && code(activeToken);
            break;
        case KW_FUNC:
            // 3) <code> -> <definition> EOL <code>
            codeStatus = definition(activeToken);

            // verification of: EOF
            if (activeToken->tokenType == T_EOF){ 
                printf("Leaving function code()...\n");
                return codeStatus;
            }

            // verification of: EOL
            if (activeToken->tokenType != T_EOL){ 
                printf("Leaving function code()...\n");
                return false;
            }

            getNextToken(activeToken);
            codeStatus = codeStatus && code(activeToken);
            break;
        case T_EOF:
            // 5) <code> -> EOF
            codeStatus = true;
            break;
        default:
            printf("Leaving function code()...\n");
            return false;
        
    }
    printf("Leaving function code()...\n");
    return codeStatus;
}

bool eol(token *activeToken){
    bool eolStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function eol()...\n");

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
            break;
        case T_EOL:
            // 6) <eol> -> EOL
            getNextToken(activeToken);
            eolStatus = true;
            break;
        default:
            printf("Leaving function eol()...\n");
            return false;
    }
    printf("Leaving function eol()...\n");
    return eolStatus;
}

bool type(token *activeToken){
    bool typeStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function type()...\n");

    //TO DO nevím jestli jsou to ty správné tokeny na typy
    switch(activeToken->tokenType) {
        case KW_INT:
            // 8) <type> -> int
            getNextToken(activeToken);
            typeStatus = true;
            break;
        case KW_DOUBLE:
            // 9) <type> -> double
            getNextToken(activeToken);
            typeStatus = true;
            break;
        case KW_STRING:
            // 10) <type> -> string
            getNextToken(activeToken);
            typeStatus = true;
            break;
        default: 
            printf("Leaving function type()...\n");
            return false;
    }
    printf("Leaving function type()...\n");
    return typeStatus;
}

bool definition(token *activeToken){
    bool definitionStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function definition()...\n");

    switch(activeToken->tokenType) {
        case KW_FUNC:
            // 11) <definition> -> func ID (<functionParams>) <funcDefMid>

            // verification of: func ID 
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){ 
                printf("Leaving function definition()...\n");
                return false;
            }

            // verification of: func ID(
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){ 
                printf("Leaving function definition()...\n");
                return false;
            }

            // verification of: func ID(<functionParams>
            getNextToken(activeToken);
            definitionStatus = functionParams(activeToken);

            // verification of: func ID(<functionParams>) <funcDefMid>
            definitionStatus = definitionStatus && funcDefMid(activeToken);
            break;
        default:
            printf("Leaving function definition()...\n");
            return false;
    }
    printf("Leaving function definition()...\n");
    return definitionStatus;
}

bool funcDefMid(token *activeToken){
    bool funcDefMidStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function funcDefMid()...\n");

    switch(activeToken->tokenType) {
        case T_LEFT_CURLY_BRACKET:
            // 12) <funcDefMid> -> {<statements>}
            getNextToken(activeToken);
            funcDefMidStatus = statements(activeToken);

            // verification of: right curly bracket
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function funcDefMid()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case T_ARROW:
            // 13) <funcDefMid> -> -> <type> {<statements>}
            getNextToken(activeToken);
            funcDefMidStatus = type(activeToken);

            // verification of: left curly bracket
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function funcDefMid()...\n");
                return false;
            }

            getNextToken(activeToken);
            funcDefMidStatus = funcDefMidStatus && statements(activeToken);

            // verification of: right curly bracket
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function funcDefMid()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        default:
            printf("Leaving function funcDefMid()...\n");
            return false;
    }
    printf("Leaving function funcDefMid()...\n");
    return funcDefMidStatus;
}

bool functionParams(token *activeToken){
    bool functionParamsStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function functionParams()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_UNDERSCORE:
            // 15) <functionParams> -> <functionParam> <functionParamsN>
            functionParamsStatus = functionParam(activeToken) && functionParamsN(activeToken);
            break;
        case T_RIGHT_BRACKET:
            // 14) <functionParams> -> EPS
            getNextToken(activeToken);
            functionParamsStatus = true;
            break;
        default:
            printf("Leaving function functionParams()...\n");
            return false;
    }
    printf("Leaving function functionParams()...\n");
    return functionParamsStatus;
}

bool functionParamsN(token *activeToken){
    bool functionParamsNStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function functionParamsN()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
            // 17) <functionParamsN> -> EPS
            getNextToken(activeToken);
            functionParamsNStatus = true;
            break;
        case T_COMMA:
            // 16) <functionParamsN> -> , <functionParam> <functionParamsN>
            getNextToken(activeToken);
            functionParamsNStatus = functionParam(activeToken) && functionParamsN(activeToken);
            break;
        default:
            printf("Leaving function functionParamsN()...\n");
            return false;
    }
    printf("Leaving function functionParamsN()...\n");
    return functionParamsNStatus;
}

bool functionParam(token *activeToken){
    bool functionParamStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function functionParam()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 19) <functionParam> -> ID ID : <type>

            // verification of: second ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            // verification of: colon
            getNextToken(activeToken);
            if (activeToken->tokenType != T_COLON){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            getNextToken(activeToken);
            functionParamStatus = type(activeToken);
            break;
        case KW_UNDERSCORE:
            // 18) <functionParam> -> _ ID : <type>
            
            // verification of: ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            // verification of: colon
            getNextToken(activeToken);
            if (activeToken->tokenType != T_COLON){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            getNextToken(activeToken);
            functionParamStatus = type(activeToken);
            break;
        default:
            printf("Leaving function functionParam()...\n");
            return false;
    }
    printf("Leaving function functionParam()...\n");
    return functionParamStatus;
}

bool statements(token *activeToken){
    bool statementsStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function statements()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
            // 20) <statements> -> <statement> EOL <statements>
            statementsStatus = statement(activeToken);

            // verification of: }
            if (activeToken->tokenType == T_RIGHT_CURLY_BRACKET){ 
                printf("Leaving function statements()...\n");
                return statementsStatus;
            }

            // verification of: EOL
            if (activeToken->tokenType != T_EOL){ 
                printf("Leaving function statements()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementsStatus = statementsStatus && statements(activeToken);
            break;
        case T_RIGHT_CURLY_BRACKET:
            // 21) <statements> -> EPS
            getNextToken(activeToken);
            statementsStatus = true;
            break;
        default:
            printf("Leaving function statements()...\n");
            return false;
    }
    printf("Leaving function statements()...\n");
    return statementsStatus;
}

bool statement(token *activeToken){
    bool statementStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function statement()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 26) <statement> -> ID <callOrAssign>
            getNextToken(activeToken);
            statementStatus = callOrAssign(activeToken);
            break;
        case KW_RETURN:
            // 25) <statement> -> return <returnExpression>
            getNextToken(activeToken);
            statementStatus = returnExpression(activeToken);
            break;
        case KW_IF:
            // 23) <statement> -> if <expression> {<statements>} else {<statements>}
            getNextToken(activeToken);
            statementStatus = expression(activeToken);  //TO DO jak to bude s expression (má potom být další getNextToken())

            // verification of: {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: }
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            // verification of: else
            getNextToken(activeToken);
            if (activeToken->tokenType != KW_ELSE){
                printf("Leaving function statement()...\n");
                return false;
            }

            // verification of: {
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: }
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_WHILE:
            // 24) <statement> -> while <expression> {<statements>}
            getNextToken(activeToken);
            statementStatus = expression(activeToken);

            // verification of: {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: }
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_LET:
        case KW_VAR:
            // 22) <statement> -> <varDec>
            statementStatus = varDec(activeToken);
            break;
        default:
            printf("Leaving function statement()...\n");
            return false;  
    }
    printf("Leaving function statement()...\n");
    return statementStatus;
}

bool callOrAssign(token *activeToken){
    bool callOrAssignStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function callOrAssign()...\n");

    switch(activeToken->tokenType) {
        case T_LEFT_BRACKET:
            // 28) <callOrAssign> -> (<arguments>)
            getNextToken(activeToken);
            callOrAssignStatus = arguments(activeToken);
            break;
        case T_ASSIGNMENT:
            // 27) <callOrAssign> -> = <expression>
            getNextToken(activeToken);
            callOrAssignStatus = expression(activeToken);
            break;
        default:
            printf("Leaving function callOrAssign()...\n");
            return false;
    }
    printf("Leaving function callOrAssign()...\n");
    return callOrAssignStatus;
}

bool varDec(token *activeToken){
    bool varDecStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function varDec()...\n");

    switch(activeToken->tokenType) {
        case KW_LET:
            // 29) <varDec> -> let ID <varDecMid>
            
            // verification of: ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function varDec()...\n");
                return false;
            }

            getNextToken(activeToken);
            varDecStatus = varDecMid(activeToken);
            break;
        case KW_VAR:
            // 30) <varDec> -> var ID <varDecMid>
            
            // verification of: ID
            getNextToken(activeToken);
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function varDec()...\n");
                return false;
            }

            getNextToken(activeToken);
            varDecStatus = varDecMid(activeToken);
            break;
        default:
            printf("Leaving function varDec()...\n");
            return false;
    }
    printf("Leaving function varDec()...\n");
    return varDecStatus;
}

bool varDecMid(token *activeToken){
    bool varDecMidStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function varDecMid()...\n");

    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
            // 32) <varDecMid> -> = <expression>
            getNextToken(activeToken);
            varDecMidStatus = expression(activeToken);
            break;
        case T_COLON:
            // 31) <varDecMid> -> : <type> <varDef>
            getNextToken(activeToken);
            varDecMidStatus = type(activeToken) && varDef(activeToken);
            break;
        default:
            printf("Leaving function varDecMid()...\n");
            return false;
    }
    printf("Leaving function varDecMid()...\n");
    return varDecMidStatus;
}

bool varDef(token *activeToken){
    bool varDefStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function varDef()...\n");

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
            break;
        case T_ASSIGNMENT:
            // 34) <varDef> -> = <expression>
            getNextToken(activeToken);
            varDefStatus = expression(activeToken);
            break;
        default:
            printf("Leaving function varDef()...\n");
            return false; 
    }
    printf("Leaving function varDef()...\n");
    return varDefStatus;
}

bool returnExpression(token *activeToken){
    bool returnExpressionStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function returnExpression()...\n");

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
            break;
        /*case <expression>:  //TO DO co s tím
            // 35) <returnExpression> -> <expression>
            returnExpressionStatus = expression(activeToken);
            break;
        */
        default:
            printf("Leaving function returnExpression()...\n");
            return false;
    }
    printf("Leaving function returnExpression()...\n");
    return returnExpressionStatus;
}

bool arguments(token *activeToken){
    bool argumentsStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function arguments()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 38) <arguments> -> <argument> <argumentsN>
            argumentsStatus = argument(activeToken) && argumentsN(activeToken);
            break;
        case T_RIGHT_BRACKET:
            // 37) <arguments> -> EPS
            getNextToken(activeToken);
            //printf("Tokennnn: %s\n",getTokenName(activeToken->tokenType));
            argumentsStatus = true;
            break;
        default:
            printf("Leaving function arguments()...\n");
            return false;
    }
    printf("Leaving function arguments()...\n");
    return argumentsStatus;
}

bool argumentsN(token *activeToken){
    bool argumentsNStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function argumentsN()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
            // 40) <argumentsN> -> EPS
            getNextToken(activeToken);
            argumentsNStatus = true;
            break;
        case T_COMMA:
            // 39) <argumentsN> -> , <argument> <argumentsN>
            getNextToken(activeToken);
            argumentsNStatus = argument(activeToken) && argumentsN(activeToken);
            break;
        default:
            printf("Leaving function argumentsN()...\n");
            return false;
    }
    printf("Leaving function argumentsN()...\n");
    return argumentsNStatus;
}

bool argument(token *activeToken){
    bool argumentStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function argument()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 42) <argument> -> ID <argWithName>
            getNextToken(activeToken);
            argumentStatus = argWithName(activeToken);
            break;
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 41) <argument> -> <dataType>
            argumentStatus = dataType(activeToken);
            break;
        case T_RIGHT_BRACKET:
        case T_COMMA:
            // 43) <argument> -> EPS
            getNextToken(activeToken);
            argumentStatus = true;
            break;
        default:
            printf("Leaving function argument()...\n");    
            return false;
    }
    printf("Leaving function argument()...\n");    
    return argumentStatus;
}

bool argWithName(token *activeToken){
    bool argWithNameStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function argWithName()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
        case T_COMMA:
            // 45) <argWithName> -> EPS
            getNextToken(activeToken);
            argWithNameStatus = true;
            break;
        case T_COLON:
            // 44) <argWithName> -> : <argumentType>
            getNextToken(activeToken);
            argWithNameStatus = argumentType(activeToken);
            break;
        default:
            printf("Leaving function argWithName()...\n");
            return false;
    }
    printf("Leaving function argWithName()...\n");
    return argWithNameStatus;
}

bool argumentType(token *activeToken){
    bool argumentTypeStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function argumentType()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
            // 46) <argumentType> -> ID
            getNextToken(activeToken);
            argumentTypeStatus = true;
            break;
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 47) <argumentType> -> <dataType>
            argumentTypeStatus = dataType(activeToken);
            break;
        default:
            printf("Leaving function argumentType()...\n");
            return false;
    }
    printf("Leaving function argumentType()...\n");
    return argumentTypeStatus;
}

bool dataType(token *activeToken){
    bool dataTypeStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function dataType()...\n");

    switch(activeToken->tokenType) {
        case T_INT:
            // 48) <dataType> -> IntData
            getNextToken(activeToken);
            dataTypeStatus = true;
            break;
        case T_DOUBLE:
            // 50) <dataType> -> DoubleData
            getNextToken(activeToken);
            dataTypeStatus = true;
            break;
        case T_STRING:
            // 49) <dataType> -> StringData
            getNextToken(activeToken);
            dataTypeStatus = true;
            break;
        default:
            return false;
    }
    printf("Leaving function dataType()...\n");
    return dataTypeStatus;
}

bool expression(token *activeToken){
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function expression()...\n");

    getNextToken(activeToken);
    printf("Leaving function expression()...\n");
    return true;
}
