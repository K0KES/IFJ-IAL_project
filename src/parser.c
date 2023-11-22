#include "parser.h"

enum tokenType typeOfLastToken;
symtable *symTable;
generator *gen;

int tokenInit(token *activeToken){
    activeToken = (token*)malloc(sizeof(token));
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
    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;
    return 0;
}

int tokenFree(token *activeToken){
    if (activeToken == NULL) return 99;

    if(activeToken->position != NULL){
        free(activeToken->position);
        activeToken->position = NULL;
    }

    if(activeToken->value != NULL){
        free(activeToken->value);
        activeToken->value = NULL;
    }
    free(activeToken);
    activeToken = NULL;
    return 0;
}

int parse(token *activeToken, symtable *symTablePtr, generator *genPtr){
    symTable = symTablePtr;
    gen = genPtr;
    getNextToken(activeToken);
    if(start(activeToken)){
        printf("Success\n");
        return 0;
    }
    else{
        printf("Error\n");
        raiseError(ERR_SYNTAX);
        return 2;
    }
}

int getNextToken(token *activeToken){
    typeOfLastToken = activeToken->tokenType;
    tokenClear(activeToken);
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
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_WRITE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
            // 1) <start> -> <eol> <code>
            startStatus = eol(activeToken) && code(activeToken);
            break;
        case T_END:
            // 2) <start> -> EPS
            getNextToken(activeToken);  //TO DO maybe verify if non token is after END
            startStatus = true;
            break;
        default:  
            printf("Leaving function start() with %d ...\n",false);
            return false;
    }
    printf("Leaving function start() with %d ...\n",startStatus);
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
                printf("Leaving function code() with %d ...\n",codeStatus);
                return codeStatus;
            }

            // verification of: EOL
            if (typeOfLastToken != T_EOL){ 
                if (activeToken->tokenType != T_EOL){ 
                    printf("Leaving function code() with %d ...\n",false);
                    return false;
                }
                getNextToken(activeToken);
            }

            codeStatus = codeStatus && code(activeToken);
            break;
        case T_IDENTIFIER:
        case KW_RETURN:
        case KW_IF:
        case KW_WHILE:
        case KW_LET:
        case KW_VAR:
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_WRITE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
            // 4) <code> -> <statement> EOL/EOF <code>
            codeStatus = statement(activeToken);

            // verification of: EOF
            if (activeToken->tokenType == T_EOF){ 
                printf("Leaving function code() with %d ...\n",codeStatus);
                return codeStatus;
            }

            // verification of: EOL
            if (typeOfLastToken != T_EOL){ 
                if (activeToken->tokenType != T_EOL){ 
                    printf("Leaving function code() with %d ...\n",false);
                    return false;
                }
                getNextToken(activeToken);
            }
            
            codeStatus = codeStatus && code(activeToken);
            break;
        case T_EOF:
            // 5) <code> -> EOF
            codeStatus = true;
            break;
        default:
            printf("Leaving function code() with %d ...\n",false);
            return false;
        
    }
    printf("Leaving function code() with %d ...\n",codeStatus);
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
        case T_INCREMENT:
        case T_DECREMENT:
        case T_VAR_MUL_VAR:
        case T_VAR_DIV_VAR:
        case T_ARROW:
        case T_COLON:
        case KW_UNDERSCORE:
        case T_COMMA:
        case T_EOF:
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_WRITE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
            // 7) <eol> -> EPS
            eolStatus = true;
            break;
        default:
            printf("Leaving function eol() with %d ...\n",false);
            return false;
    }
    printf("Leaving function eol() with %d ...\n",eolStatus);
    return eolStatus;
}

bool type(token *activeToken){
    bool typeStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function type()...\n");

    switch(activeToken->tokenType) {
        case KW_INT:
            // 8) <type> -> int
            getNextToken(activeToken);
            if (activeToken->tokenType == T_NULLABLE){
                //edit symtableSetVariableType(symTable, DATA_TYPE_INTEGER, true);
                getNextToken(activeToken);
            }
            else {
                //symtableSetVariableType(symTable, DATA_TYPE_INTEGER, false);
            }
            typeStatus = true;
            break;
        case KW_DOUBLE:
            // 9) <type> -> double
            getNextToken(activeToken);
            if (activeToken->tokenType == T_NULLABLE){
                //symtableSetVariableType(symTable, DATA_TYPE_DOUBLE, true);
                getNextToken(activeToken);
            }
            else {
                //symtableSetVariableType(symTable, DATA_TYPE_DOUBLE, false);
            }
            typeStatus = true;
            break;
        case KW_STRING:
            // 10) <type> -> string
            getNextToken(activeToken);
            if (activeToken->tokenType == T_NULLABLE){
                //symtableSetVariableType(symTable, DATA_TYPE_STRING, true);
                getNextToken(activeToken);
            }
            else {
                //symtableSetVariableType(symTable, DATA_TYPE_STRING, false);
            }
            typeStatus = true;
            break;
        default: 
            printf("Leaving function type() with %d ...\n",false);
            return false;
    }
    printf("Leaving function type() with %d ...\n",typeStatus);
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
                printf("Leaving function definition() with %d ...\n",false);
                return false;
            }

            //Add function to symtable
            symtableInsert(symTable,activeToken->value->str,true);
            symtableEnterScope(symTable,activeToken->value->str,NULL);

            // verification of: func <eol> ID <eol>
            getNextToken(activeToken);
            definitionStatus = definitionStatus && eol(activeToken);

            // verification of: func <eol> ID <eol> (
            if (activeToken->tokenType != T_LEFT_BRACKET){ 
                printf("Leaving function definition() with %d ...\n",false);
                return false;
            }

            // verification of: func <eol> ID <eol> (<functionParams>
            getNextToken(activeToken);
            definitionStatus = definitionStatus && functionParams(activeToken);

            //TO DO symtable symtableFunctionEndOfArguments()

            // verification of: func <eol> ID <eol> (<functionParams>) <eol>
            definitionStatus = definitionStatus && eol(activeToken);

            // verification of: func <eol> ID <eol> (<functionParams>) <eol> <funcDefMid>
            definitionStatus = definitionStatus && funcDefMid(activeToken);
            // debug symtablePrintVariables(symTable);
            symtableExitScope(symTable);
            break;
        default:
            printf("Leaving function definition() with %d ...\n",false);
            return false;
    }
    printf("Leaving function definition() with %d ...\n",definitionStatus);
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

            //getNextToken(activeToken);
            break;
        case T_ARROW:
            // 13) <funcDefMid> -> -> <eol> <type> <eol> {<statements>}
            getNextToken(activeToken);
            funcDefMidStatus = eol(activeToken) && type(activeToken) && eol(activeToken);

            // verification of: left curly bracket
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function funcDefMid() with %d ...\n",false);
                return false;
            }

            getNextToken(activeToken);
            funcDefMidStatus = funcDefMidStatus && statements(activeToken);
            break;
        default:
            printf("Leaving function funcDefMid() with %d ...\n",false);
            return false;
    }
    printf("Leaving function funcDefMid() with %d ...\n",funcDefMidStatus);
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
            printf("Leaving function functionParams() with %d ...\n",false);
            return false;
    }
    printf("Leaving function functionParams() with %d ...\n",functionParamsStatus);
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
            printf("Leaving function functionParamsN() with %d ...\n",false);
            return false;
    }
    printf("Leaving function functionParamsN() with %d ...\n",functionParamsNStatus);
    return functionParamsNStatus;
}

bool functionParam(token *activeToken){
    bool functionParamStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function functionParam()...\n");

    symtableAddFunctionNextArgument(symTable);

    switch(activeToken->tokenType) {
        case KW_UNDERSCORE:
            // 19) <functionParam> -> _ <eol> ID <eol> : <eol> <type> <eol>
            
            symtableSetFunctionArgumentName(symTable,"_");

            // verification of: _ <eol>
            getNextToken(activeToken);
            functionParamStatus = eol(activeToken);

            // verification of: _ <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function functionParam() with %d ...\n",false);
                return false;
            }

            //edit symtableSetFunctionArgumentID(symTable,activeToken->value->str);

            // verification of: _ <eol> ID <eol>
            getNextToken(activeToken);
            functionParamStatus = functionParamStatus && eol(activeToken);

            // verification of: _ <eol> ID <eol> :
            if (activeToken->tokenType != T_COLON){
                printf("Leaving function functionParam() with %d no colon...\n",false);
                return false;
            }

            // verification of: _ <eol> ID <eol> : <eol> <type> <eol>
            getNextToken(activeToken);
            functionParamStatus = functionParamStatus && eol(activeToken) && type(activeToken) && eol(activeToken);
            break;
        case T_IDENTIFIER:
            // 20) <functionParam> -> ID <eol> ID <eol> : <eol> <type> <eol>

            symtableSetFunctionArgumentName(symTable,activeToken->value->str);

            // verification of: ID <eol>
            getNextToken(activeToken);
            functionParamStatus = eol(activeToken);

            // verification of: ID <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function functionParam() with %d ...\n",false);
                return false;
            }

            //edit symtableSetFunctionArgumentID(symTable,activeToken->value->str);

            // verification of: ID <eol> ID <eol>
            getNextToken(activeToken);
            functionParamStatus = functionParamStatus && eol(activeToken);

            // verification of: ID <eol> ID <eol> :
            if (activeToken->tokenType != T_COLON){
                printf("Leaving function functionParam() with %d ...\n",false);
                return false;
            }
            // verification of: ID <eol> ID <eol> : <eol> <type> <eol>
            getNextToken(activeToken);
            functionParamStatus = eol(activeToken) && type(activeToken) && eol(activeToken);
            break;
        default:
            printf("Leaving function functionParam() with %d ...\n",false);
            return false;
    }
    printf("Leaving function functionParam() with %d ...\n",functionParamStatus);
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
        case T_EOL:
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_WRITE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
            // 22) <statements> -> EOL <statementBlock> <eol>
            statementsStatus = eol(activeToken) && statementsBlock(activeToken) && eol(activeToken);
            break;
        case T_RIGHT_CURLY_BRACKET:
            // 23) <statements> -> EPS
            getNextToken(activeToken);
            statementsStatus = true;
            break;
        default:
            printf("Leaving function statements() with %d ...\n",false);
            return false;
    }
    printf("Leaving function statements() with %d ...\n",statementsStatus);
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
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_WRITE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
            // 24) <statementsBlock> -> <statement> EOL <statementsBlock>
            statementsBlockStatus = statement(activeToken);

            // verification of: <statement> EOL
            if (typeOfLastToken != T_EOL && activeToken->tokenType != T_RIGHT_CURLY_BRACKET){ 
                if (activeToken->tokenType != T_EOL){ 
                    printf("Leaving function statementsBlock() with %d ...\n",false);
                    return false;
                }
                getNextToken(activeToken);
            }

            statementsBlockStatus = statementsBlockStatus && statementsBlock(activeToken);
            break;
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 25) <statementsBlock> -> EPS
            getNextToken(activeToken);
            statementsBlockStatus = true;
            break;
        default:
            printf("Leaving function statementsBlock() with %d ...\n",false);
            return false;
    }
    printf("Leaving function statementsBlock() with %d ...\n",statementsBlockStatus);
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
            // 27) <statement> -> if <eol> <expression> <eol> {<statements>} <eol> else <eol> {<statements>}
            
            symtableEnterScope(symTable,"if",NULL);
            
            getNextToken(activeToken);
            statementStatus = eol(activeToken) && expression(activeToken) && eol(activeToken);  //TO DO jak to bude s expression (má potom být další getNextToken())

            // verification of: if <eol>  <expression> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> 
            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken) && eol(activeToken);

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> else
            if (activeToken->tokenType != KW_ELSE){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> else <eol>
            getNextToken(activeToken);
            statementStatus = statementStatus && eol(activeToken);

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> else <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            symtableExitScope(symTable);

            break;
        case KW_WHILE:
            // 28) <statement> -> while <eol> <expression> <eol> {<statements>}

            symtableEnterScope(symTable,"while",NULL);

            getNextToken(activeToken);
            statementStatus = eol(activeToken) && expression(activeToken) && eol(activeToken);

            // verification of: while <eol>  <expression> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }

            getNextToken(activeToken);
            statementStatus = statementStatus && statements(activeToken);

            symtableExitScope(symTable);

            break;
        case KW_RETURN:
            // 29) <statement> -> return <returnExpression>
            getNextToken(activeToken);
            //TO DO expression parser has to check if returnExpression is same type as return type of function
            statementStatus = returnExpression(activeToken);
            break;
        case T_IDENTIFIER:
            // 30) <statement> -> ID <callOrAssign>
            //TO DO symtable kontrola symtable jestli je var/func definovaná 
            getNextToken(activeToken);
            statementStatus = callOrAssign(activeToken);
            break;
        case KW_READSTRING:
            // 66) <statement> -> KW_VESTAVENA_FUNKC ()
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = true;
            break;
        case KW_READINT:
            // 66) <statement> -> KW_VESTAVENA_FUNKC ()
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = true;
            break;
        case KW_READDOUBLE:
            // 66) <statement> -> KW_VESTAVENA_FUNKC ()
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = true;
            break;
        case KW_WRITE:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<arguments>)
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = arguments(activeToken);
            break;
        case KW_INT_TO_DOUBLE:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = argument(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_DOUBLE_TO_INT:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = argument(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_LENGTH:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = argument(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_SUBSTRING:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>)
            // verification of: KW_VESTAVENA_FUNKCE (<argument>
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = argument(activeToken);
            
            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = statementStatus && argument(activeToken);

            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = statementStatus && argument(activeToken);

            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>)
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_ORD:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = argument(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            break;
        case KW_CHR:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken(activeToken);
            if (activeToken->tokenType != T_LEFT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            statementStatus = argument(activeToken);
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                printf("Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken(activeToken);
            break;
        default:
            printf("Leaving function statement() with %d ...\n",false);
            return false;  
    }
    printf("Leaving function statement() with %d ...\n",statementStatus);
    return statementStatus;
}

bool callOrAssign(token *activeToken){
    bool callOrAssignStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function callOrAssign()...\n");

    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
        case T_INCREMENT:
        case T_DECREMENT:
        case T_VAR_MUL_VAR:
        case T_VAR_DIV_VAR:
        case T_EOL:
            // 36) <callOrAssign> -> <eol> <assign>>
            callOrAssignStatus = eol(activeToken) && assign(activeToken);
            break;
        case T_LEFT_BRACKET:
            // 37) <callOrAssign> -> (<arguments>)
            getNextToken(activeToken);
            callOrAssignStatus = arguments(activeToken);
            break;
        default:
            printf("Leaving function callOrAssign() with %d ...\n",false);
            return false;
    }
    printf("Leaving function callOrAssign() with %d ...\n",callOrAssignStatus);
    return callOrAssignStatus;
}

bool assign(token *activeToken){
    bool assignStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function assign()...\n");

    //TO DO symtable pouze assignment jde u všech datových typů -> ošetřit
    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
            // 61) <assign> -> = <expression>
            getNextToken(activeToken);
            assignStatus = expression(activeToken);
            break;
        case T_INCREMENT:
            // 62) <assign> -> += <expression>
            getNextToken(activeToken);
            assignStatus = expression(activeToken);
            break;
        case T_DECREMENT:
            // 63) <assign> -> -= <expression>
            getNextToken(activeToken);
            assignStatus = expression(activeToken);
            break;
        case T_VAR_MUL_VAR:
            // 64) <assign> -> *= <expression>
            getNextToken(activeToken);
            assignStatus = expression(activeToken);
            break;
        case T_VAR_DIV_VAR:
            // 65) <assign> -> /= <expression>
            getNextToken(activeToken);
            assignStatus = expression(activeToken);
            break;
        default:
            printf("Leaving function assign() with %d ...\n",false);
            return false;
    }
    printf("Leaving function assign() with %d ...\n",assignStatus);
    return assignStatus;
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
                printf("Leaving function varDec() with %d ...\n",false);
                return false;
            }

            symtableInsert(symTable,activeToken->value->str,false);

            getNextToken(activeToken);
            varDecStatus = eol(activeToken) && varDecMid(activeToken);
            break;
        case KW_VAR:
            // 39) <varDec> -> var <eol> ID <eol> <varDecMid>

            getNextToken(activeToken);
            varDecStatus = eol(activeToken);
            
            // verification of: var <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                printf("Leaving function varDec() with %d ...\n",false);
                return false;
            }

            symtableInsert(symTable,activeToken->value->str,false);
            // TO DO přidat do vygenerovaného kodu - DEFVAR activeToken->value->str
            //generatorPushStringToList(gen->mainCode,strcat("DEFVAR GF@",activeToken->value->str));

            generatorPushStringToList(gen->mainCode,concatString(2,"DEFVAR GF@",activeToken->value->str));
            //TO DO Generování uložit id na stack
            //generatorPushStringToList(gen->parserStack,strcat());
            generatorPushStringToList(gen->parserStack,concatString(2,"GF@",activeToken->value->str));
            

            getNextToken(activeToken);
            varDecStatus = eol(activeToken) && varDecMid(activeToken);
            break;
        default:
            printf("Leaving function varDec() with %d ...\n",false);
            return false;
    }
    printf("Leaving function varDec() with %d ...\n",varDecStatus);
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
            //TO DO symtable get type of expression and set it as type of active item 
            break;
        default:
            printf("Leaving function varDecMid() with %d ...\n",false);
            return false;
    }
    printf("Leaving function varDecMid() with %d ...\n",varDecMidStatus);
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
            //TO DO symtable check if expression has same type as variable
            varDefStatus = expression(activeToken); //expression parser pushne výsledek výrazu na zásobník 
            generatorPushStringToList(gen->parserStack,"int@5");
            //TO DO generování pop 2 prvky ze zásobníku vygeneruji - MOV id1 id2
            generatorPushStringToList(gen->mainCode,concatString(4,"MOVE ",generatorPopStringFromList(gen->parserStack)," ",generatorPopStringFromList(gen->parserStack)));
            break;
        default:
            // verification of: EOL
            if (typeOfLastToken == T_EOL){ 
                varDefStatus = true;
                break;
            }
            printf("Leaving function varDef() with %d ...\n",false);
            return false; 
    }
    printf("Leaving function varDef() with %d ...\n",varDefStatus);
    return varDefStatus;
}

bool returnExpression(token *activeToken){
    bool returnExpressionStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function returnExpression()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 45) <returnExpression> -> EPS
            returnExpressionStatus = true;
            break;
        default:
            // 44) <returnExpression> -> <expression>
            //TO DO symtable check if function has same return type as is type of expression
            returnExpressionStatus = expression(activeToken);
            break;
    }
    printf("Leaving function returnExpression() with %d ...\n",returnExpressionStatus);
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
            argumentsStatus = argument(activeToken) && argumentsN(activeToken); //&& eol(activeToken)
            break;
        default:
            printf("Leaving function arguments() with %d ...\n",false);
            return false;
    }
    printf("Leaving function arguments() with %d ...\n",argumentsStatus);
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
            printf("Leaving function argumentsN() with %d ...\n",false);
            return false;
    }
    printf("Leaving function argumentsN() with %d ...\n",argumentsNStatus);
    return argumentsNStatus;
}

bool argument(token *activeToken){
    bool argumentStatus = false;
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function argument()...\n");

    //TO DO symtable check if parametrs if function are same type as arguments
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
            printf("Leaving function argument() with %d ...\n",false);
            return false;
    }
    printf("Leaving function argument() with %d ...\n",argumentStatus);
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
            //getNextToken(activeToken);
            argWithNameStatus = true;
            break;
        default:
            printf("Leaving function argWithName() with %d ...\n",false);
            return false;
    }
    printf("Leaving function argWithName() with %d ...\n",argWithNameStatus);
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
            printf("Leaving function argumentType() with %d ...\n",false);
            return false;
    }
    printf("Leaving function argumentType() with %d ...\n",argumentTypeStatus);
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
    printf("Leaving function dataType() with %d ...\n",dataTypeStatus);
    return dataTypeStatus;
}

bool expression(token *activeToken){
    printf("Token: %s\n",getTokenName(activeToken->tokenType));
    printf("Entering function expression()...\n");
    if (activeToken->tokenType == T_LEFT_BRACKET){
        while (activeToken->tokenType != T_RIGHT_BRACKET){
            printf("getting next\n");
            getNextToken(activeToken);
        }
    }
    
    getNextToken(activeToken);
    printf("Leaving function expression() with %d ...\n",true);
    return true;
}
