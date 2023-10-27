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
        case T_EOL:
            // 1) <start> -> <eol> <code>
            startStatus = eol(activeToken) && code(activeToken);
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
        case KW_FUNC:
            // 3) <code> -> <definition> EOL/EOF <code>
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
        case T_EOL:
            // 6) <eol> -> EOL
            getNextToken(activeToken);
            eolStatus = true;
            break;
        case T_IDENTIFIER:
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
        case KW_FUNC:
        case KW_RETURN:
        case KW_IF:
        case KW_ELSE:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
        case KW_INT:
        case KW_DOUBLE:
        case KW_STRING:
        case T_LEFT_BRACKET:
        case T_RIGHT_BRACKET:
        case T_LEFT_CURLY_BRACKET:
        case T_RIGHT_CURLY_BRACKET:
        case T_ASSIGNMENT:
        case T_ARROW:
        case T_COLON:
        case KW_UNDERSCORE:
        case T_COMMA:
        case T_EOF:
            // 7) <eol> -> EPS
            //getNextToken(activeToken);  //TO DO maybe verify if non token is after EOF
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
            // 11) <definition> -> func <eol> ID <eol> (<functionParams>) <eol> <funcDefMid>

            // verification of: func <eol>
            getNextToken(activeToken);
            definitionStatus = eol(activeToken);

            // verification of: func <eol> ID 
            if (activeToken->tokenType != T_IDENTIFIER){ 
                printf("Leaving function definition()...\n");
                return false;
            }

            // verification of: func <eol> ID <eol>
            getNextToken(activeToken);
            definitionStatus = definitionStatus && eol(activeToken);

            // verification of: func <eol> ID <eol> (
            if (activeToken->tokenType != T_LEFT_BRACKET){ 
                printf("Leaving function definition()...\n");
                return false;
            }

            // verification of: func <eol> ID <eol> (<functionParams>
            getNextToken(activeToken);
            definitionStatus = definitionStatus && functionParams(activeToken);

            // verification of: func <eol> ID <eol> (<functionParams>) <eol>
            definitionStatus = definitionStatus && eol(activeToken);

            // verification of: func <eol> ID <eol> (<functionParams>) <eol> <funcDefMid>
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

            getNextToken(activeToken);
            break;
        case T_ARROW:
            // 13) <funcDefMid> -> -> <eol> <type> <eol> {<statements>}
            getNextToken(activeToken);
            funcDefMidStatus = eol(activeToken) && type(activeToken) && eol(activeToken);

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
        case T_RIGHT_BRACKET:
            // 14) <functionParams> -> EPS
            getNextToken(activeToken);
            functionParamsStatus = true;
            break;
        case T_EOL:
            // 15) <functionParams> -> EOL <functionParams>
            getNextToken(activeToken);
            functionParamsStatus = functionParams(activeToken);
            break;
        case T_IDENTIFIER:
        case KW_UNDERSCORE:
            // 16) <functionParams> -> <functionParam> <functionParamsN> <eol>
            functionParamsStatus = functionParam(activeToken) && functionParamsN(activeToken) && eol(activeToken);
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
        case T_COMMA:
            // 17) <functionParamsN> -> , <eol> <functionParam> <functionParamsN>
            getNextToken(activeToken);
            functionParamsNStatus = eol(activeToken) && functionParam(activeToken) && functionParamsN(activeToken);
            break;
        case T_RIGHT_BRACKET:
        case T_EOL:
            // 18) <functionParamsN> -> EPS
            getNextToken(activeToken);
            functionParamsNStatus = true;
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
        case KW_UNDERSCORE:
            // 19) <functionParam> -> _ <eol> ID <eol> : <eol> <type> <eol>
            
            // verification of: _ <eol>
            getNextToken(activeToken);
            functionParamStatus = eol(activeToken);

            // verification of: _ <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            // verification of: _ <eol> ID <eol>
            getNextToken(activeToken);
            functionParamStatus = functionParamStatus && eol(activeToken);

            // verification of: _ <eol> ID <eol> :
            getNextToken(activeToken);
            if (activeToken->tokenType != T_COLON){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            // verification of: _ <eol> ID <eol> : <eol> <type> <eol>
            getNextToken(activeToken);
            functionParamStatus = functionParamStatus && eol(activeToken) && type(activeToken) && eol(activeToken);
            break;
        case T_IDENTIFIER:
            // 20) <functionParam> -> ID <eol> ID <eol> : <eol> <type> <eol>

            // verification of: ID <eol>
            getNextToken(activeToken);
            functionParamStatus = eol(activeToken);

            // verification of: ID <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function functionParam()...\n");
                return false;
            }

            // verification of: ID <eol> ID <eol>
            getNextToken(activeToken);
            functionParamStatus = functionParamStatus && eol(activeToken);

            // verification of: ID <eol> ID <eol> :
            getNextToken(activeToken);
            if (activeToken->tokenType != T_COLON){
                printf("Leaving function functionParam()...\n");
                return false;
            }
            // verification of: ID <eol> ID <eol> : <eol> <type> <eol>
            getNextToken(activeToken);
            functionParamStatus = eol(activeToken) && type(activeToken) && eol(activeToken);
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
            // 21) <statements> -> <statementOnLine>
            statementsStatus = statementOnLine(activeToken);
            break;
        case T_EOL:
            // 22) <statements> -> EOL <statementBlock> <eol>
            getNextToken(activeToken);
            statementsStatus = statementsBlock(activeToken) && eol(activeToken);
            break;
        case T_RIGHT_CURLY_BRACKET:
            // 23) <statements> -> EPS
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

bool statementsBlock(token *activeToken){
    bool statementsBlockStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function statementsBlock()...\n");

    switch(activeToken->tokenType) {
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
            // 24) <statementsBlock> -> <statement> EOL <statementsBlock>
            statementsBlockStatus = statement(activeToken);

            // verification of: <statement> EOL
            if (activeToken->tokenType != T_EOL){ 
                printf("Leaving function statementsBlock()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementsBlockStatus = statementsBlockStatus && statementsBlock(activeToken);
            break;
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 25) <statementsBlock> -> EPS
            getNextToken(activeToken);
            statementsBlockStatus = true;
            break;
        default:
            printf("Leaving function statementsBlock()...\n");
            return false;
    }
    printf("Leaving function statementsBlock()...\n");
    return statementsBlockStatus;
}

bool statement(token *activeToken){
    bool statementStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function statement()...\n");

    switch(activeToken->tokenType) {
        case KW_LET:
        case KW_VAR:
            // 26) <statement> -> <varDec>
            statementStatus = varDec(activeToken);
            break;
        case KW_IF:
            // 27) <statement> -> if <expression> <eol> {<statements>} <eol> else <eol> {<statements>}
            getNextToken(activeToken);
            statementStatus = expression(activeToken) && eol(activeToken);  //TO DO jak to bude s expression (má potom být další getNextToken())

            // verification of: if <expression> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: if <expression> <eol> {<statements>} 
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            // verification of: if <expression> <eol> {<statements>} <eol> 
            getNextToken(activeToken);
            statementStatus = statementStatus && eol(activeToken);

            // verification of: if <expression> <eol> {<statements>} <eol> else
            if (activeToken->tokenType != KW_ELSE){
                printf("Leaving function statement()...\n");
                return false;
            }

            // verification of: if <expression> <eol> {<statements>} <eol> else <eol>
            getNextToken(activeToken);
            statementStatus = statementStatus && eol(activeToken);

            // verification of: if <expression> <eol> {<statements>} <eol> else <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: if <expression> <eol> {<statements>} <eol> else <eol> {<statements>}
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_WHILE:
            // 28) <statement> -> while <expression> <eol> {<statements>}
            getNextToken(activeToken);
            statementStatus = expression(activeToken) && eol(activeToken);

            // verification of: while <expression> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            // verification of: while <expression> <eol> {<statements>}
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statement()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_RETURN:
            // 29) <statement> -> return <returnExpression>
            getNextToken(activeToken);
            statementStatus = returnExpression(activeToken);
            break;
        case T_IDENTIFIER:
            // 30) <statement> -> ID <callOrAssign>
            getNextToken(activeToken);
            statementStatus = callOrAssign(activeToken);
            break;
        default:
            printf("Leaving function statement()...\n");
            return false;  
    }
    printf("Leaving function statement()...\n");
    return statementStatus;
}

bool statementOnLine(token *activeToken){
    bool statementOnLineStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function statementOnLine()...\n");

    switch(activeToken->tokenType) {
        case KW_LET:
        case KW_VAR:
            // 31) <statementOnLine> -> <varDec>
            statementOnLineStatus = varDec(activeToken);
            break;
        case KW_IF:
            // 32) <statementOnLine> -> if <expression> {<statementOnLine>} <eol> else <eol> {<statementOnLine>}
            getNextToken(activeToken);
            statementOnLineStatus = expression(activeToken);  //TO DO jak to bude s expression (má potom být další getNextToken())

            // verification of: if <expression> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementOnLineStatus = statementOnLineStatus && statementOnLine(activeToken);

            // verification of: if <expression> {<statementOnLine>} 
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }

            // verification of: if <expression> {<statementOnLine>} else
            getNextToken(activeToken);
            if (activeToken->tokenType != KW_ELSE){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }

            // verification of: if <expression> {<statementOnLine>} else {
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementOnLineStatus = statementOnLineStatus && statementOnLine(activeToken);

            // verification of: if <expression> {<statementOnLine>} else {<statementOnLine>}
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_WHILE:
            // 33) <statementOnLine> -> while <expression> {<statementOnLine>}
            getNextToken(activeToken);
            statementOnLineStatus = expression(activeToken);

            // verification of: while <expression> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }

            getNextToken(activeToken);
            statementOnLineStatus = statementOnLineStatus && statementOnLine(activeToken);

            // verification of: while <expression> {<statements>}
            if (activeToken->tokenType != T_RIGHT_CURLY_BRACKET){
                printf("Leaving function statementOnLine()...\n");
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_RETURN:
            // 34) <statementOnLine> -> return <returnExpression>
            getNextToken(activeToken);
            statementOnLineStatus = returnExpression(activeToken);
            break;
        case T_IDENTIFIER:
            // 35) <statementOnLine> -> ID <callOrAssign>
            getNextToken(activeToken);
            statementOnLineStatus = callOrAssign(activeToken);
            break;
        default:
            printf("Leaving function statementOnLine()...\n");
            return false;  
    }
    printf("Leaving function statementOnLine()...\n");
    return statementOnLineStatus;
}

bool callOrAssign(token *activeToken){
    bool callOrAssignStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function callOrAssign()...\n");

    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
        case T_EOL:
            // 36) <callOrAssign> -> <eol> = <expression>
            callOrAssignStatus = eol(activeToken);

            // verification of: <eol> = 
            if (activeToken->tokenType != T_ASSIGNMENT){
                printf("Leaving function callOrAssign()...\n");
                return false;
            }

            getNextToken(activeToken);
            callOrAssignStatus = callOrAssignStatus && expression(activeToken);
            break;
        case T_LEFT_BRACKET:
            // 37) <callOrAssign> -> (<arguments>)
            getNextToken(activeToken);
            callOrAssignStatus = arguments(activeToken);
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
            // 38) <varDec> -> let <eol> ID <eol> <varDecMid>

            getNextToken(activeToken);
            varDecStatus = eol(activeToken);
            
            // verification of: let <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function varDec()...\n");
                return false;
            }

            getNextToken(activeToken);
            varDecStatus = eol(activeToken) && varDecMid(activeToken);
            break;
        case KW_VAR:
            // 39) <varDec> -> var <eol> ID <eol> <varDecMid>

            getNextToken(activeToken);
            varDecStatus = eol(activeToken);
            
            // verification of: var <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function varDec()...\n");
                return false;
            }

            getNextToken(activeToken);
            varDecStatus = eol(activeToken) && varDecMid(activeToken);
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
        case T_COLON:
            // 40) <varDecMid> -> : <eol> <type> <eol> <varDef>
            getNextToken(activeToken);
            varDecMidStatus = eol(activeToken) && type(activeToken) && eol(activeToken) && varDef(activeToken);
            break;
        case T_ASSIGNMENT:
            // 41) <varDecMid> -> = <expression>
            getNextToken(activeToken);
            varDecMidStatus = expression(activeToken);
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
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 42) <varDef> -> EPS
            getNextToken(activeToken);
            varDefStatus = true;
            break;
        case T_ASSIGNMENT:
            // 43) <varDef> -> = <expression>
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
        /*case <expression>:  //TO DO co s tím
            // 44) <returnExpression> -> <expression>
            returnExpressionStatus = expression(activeToken);
            break;
        */
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 45) <returnExpression> -> EPS
            getNextToken(activeToken);
            returnExpressionStatus = true;
            break;
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
        case T_RIGHT_BRACKET:
            // 46) <arguments> -> EPS
            getNextToken(activeToken);
            argumentsStatus = true;
            break;
        case T_EOL:
            // 47) <arguments> -> EOL <arguments>
            getNextToken(activeToken);
            argumentsStatus = arguments(activeToken);
            break;
        case T_IDENTIFIER:
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 48) <arguments> -> <argument> <argumentsN> <eol>
            argumentsStatus = argument(activeToken) && argumentsN(activeToken) && eol(activeToken);
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
        case T_COMMA:
            // 49) <argumentsN> -> , <eol> <argument> <argumentsN>
            getNextToken(activeToken);
            argumentsNStatus = eol(activeToken) && argument(activeToken) && argumentsN(activeToken);
            break;
        case T_RIGHT_BRACKET:
        case T_EOL:
            // 50) <argumentsN> -> EPS
            getNextToken(activeToken);
            argumentsNStatus = true;
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
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 51) <argument> -> <dataType> <eol>
            argumentStatus = dataType(activeToken) && eol(activeToken);
            break;
        case T_IDENTIFIER:
            // 52) <argument> -> ID <eol> <argWithName> <eol>
            getNextToken(activeToken);
            argumentStatus = eol(activeToken) && argWithName(activeToken) && eol(activeToken);
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
        case T_COLON:
            // 53) <argWithName> -> : <eol> <argumentType>
            getNextToken(activeToken);
            argWithNameStatus = eol(activeToken) && argumentType(activeToken);
            break;
        case T_RIGHT_BRACKET:
        case T_COMMA:
        case T_EOL:
            // 54) <argWithName> -> EPS
            getNextToken(activeToken);
            argWithNameStatus = true;
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
            // 55) <argumentType> -> ID
            getNextToken(activeToken);
            argumentTypeStatus = true;
            break;
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 56) <argumentType> -> <dataType>
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
            // 57) <dataType> -> IntData
            getNextToken(activeToken);
            dataTypeStatus = true;
            break;
        case T_STRING:
            // 58) <dataType> -> StringData
            getNextToken(activeToken);
            dataTypeStatus = true;
            break;
        case T_DOUBLE:
            // 59) <dataType> -> DoubleData
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
