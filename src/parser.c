#include "parser.h"

enum tokenType typeOfLastToken;

symtable *symTable;
generator *gen;
token *activeToken;
programState *state;

token *tokenInit(){
    token *activeToken = (token*)malloc(sizeof(token));
    if(activeToken == NULL){ raiseError(ERR_INTERNAL); }

    activeToken->value = (string*)malloc(sizeof(string));
    if(activeToken->value == NULL){ raiseError(ERR_INTERNAL); }

    strInit(activeToken->value);

    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    if(activeToken->position == NULL){ raiseError(ERR_INTERNAL); }

    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;
    return activeToken;
}

void tokenFree(token *activeToken ){
    if (activeToken == NULL){ raiseError(ERR_INTERNAL); }

    if(activeToken->position != NULL){
        free(activeToken->position);
        activeToken->position = NULL;
    }

    if(activeToken->value != NULL){
        strFree(activeToken->value);
        free(activeToken->value);
        activeToken->value = NULL;
    }
    free(activeToken);
    activeToken = NULL;
    return;
}

void parse(programState *programState){
    state = programState;
    symTable = programState->symTable;
    gen = programState->gen;
    activeToken = programState->activeToken;
    getNextToken();
    if(start()){
        DEBUG_PRINTF("[Parser] Success\n");
        return;
    }
    else{
        DEBUG_PRINTF("[Parser] Error\n");
        raiseError(ERR_SYNTAX);
    }
}

void getNextToken(){
    typeOfLastToken = activeToken->tokenType;
    tokenClear(activeToken);

    if (listLength(state->tokenQueue) == 0){
        DEBUG_PRINTF("[Parser] Token from scanner\n");
        getToken(activeToken, activeToken->position->charNumber, activeToken->position->lineNumber);
    }
    else {
        DEBUG_PRINTF("[Parser] Token from Queue\n");
        activeToken = listPopFirst(state->tokenQueue);
    }

    DEBUG_PRINTF("[Parser] Got next token: %s\n",getTokenName(activeToken->tokenType));
}

bool start(){
    bool startStatus = false;
    DEBUG_PRINTF("[Parser] Start token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function start()...\n");

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
            startStatus = eol() && code();
            break;
        case T_END:
            // 2) <start> -> EPS
            getNextToken();
            startStatus = true;
            break;
        default:  
            DEBUG_PRINTF("[Parser] Leaving function start() with %d ...\n",false);
            return false;
    }

    symtableEndOfFile(symTable);
    DEBUG_PRINTF("[Parser] Leaving function start() with %d ...\n",startStatus);
    return startStatus;
}

bool code(){
    bool codeStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function code()...\n");

    switch(activeToken->tokenType) {
        case KW_FUNC:
            // 3) <code> -> <definition> EOL/EOF <code>
            codeStatus = definition();

            // verification of: EOF
            if (activeToken->tokenType == T_EOF){ 
                DEBUG_PRINTF("[Parser] Leaving function code() with %d ...\n",codeStatus);
                return codeStatus;
            }

            // verification of: EOL
            if (typeOfLastToken != T_EOL){ 
                if (activeToken->tokenType != T_EOL){ 
                    DEBUG_PRINTF("[Parser] Leaving function code() with %d ...\n",false);
                    return false;
                }
                getNextToken();
            }

            codeStatus = codeStatus && code();
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
            codeStatus = statement();

            // verification of: EOF
            if (activeToken->tokenType == T_EOF){ 
                DEBUG_PRINTF("[Parser] Leaving function code() with %d ...\n",codeStatus);
                return codeStatus;
            }

            // verification of: EOL
            if (typeOfLastToken != T_EOL){ 
                if (activeToken->tokenType != T_EOL){ 
                    DEBUG_PRINTF("[Parser] Leaving function code() with %d ...\n",false);
                    return false;
                }
                getNextToken();
            }
            
            codeStatus = codeStatus && code();
            break;
        case T_EOF:
            // 5) <code> -> EOF
            codeStatus = true;
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function code() with %d ...\n",false);
            return false;
        
    }
    DEBUG_PRINTF("[Parser] Leaving function code() with %d ...\n",codeStatus);
    return codeStatus;
}

bool eol(){
    bool eolStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function eol()...\n");

    switch(activeToken->tokenType) {
        case T_EOL:
            // 6) <eol> -> EOL
            getNextToken();
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
            DEBUG_PRINTF("[Parser] Leaving function eol() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function eol() with %d ...\n",eolStatus);
    return eolStatus;
}

bool type(){
    bool typeStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function type()...\n");

    switch(activeToken->tokenType) {
        case KW_INT:
            // 8) <type> -> int
            getNextToken();
            if (activeToken->tokenType == T_NULLABLE){
                symtableSetDataType(symTable, DATA_TYPE_INTEGER, true);
                getNextToken();
            }
            else {
                symtableSetDataType(symTable, DATA_TYPE_INTEGER, false);
            }
            typeStatus = true;
            break;
        case KW_DOUBLE:
            // 9) <type> -> double
            getNextToken();
            if (activeToken->tokenType == T_NULLABLE){
                symtableSetDataType(symTable, DATA_TYPE_DOUBLE, true);
                getNextToken();
            }
            else {
                symtableSetDataType(symTable, DATA_TYPE_DOUBLE, false);
            }
            typeStatus = true;
            break;
        case KW_STRING:
            // 10) <type> -> string
            getNextToken();
            if (activeToken->tokenType == T_NULLABLE){
                symtableSetDataType(symTable, DATA_TYPE_STRING, true);
                getNextToken();
            }
            else {
                symtableSetDataType(symTable, DATA_TYPE_STRING, false);
            }
            typeStatus = true;
            break;
        default: 
            DEBUG_PRINTF("[Parser] Leaving function type() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function type() with %d ...\n",typeStatus);
    return typeStatus;
}

bool definition(){
    bool definitionStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function definition()...\n");

    switch(activeToken->tokenType) {
        case KW_FUNC:
            // 11) <definition> -> func <eol> ID <eol> (<functionParams>) <eol> <funcDefMid>

            // verification of: func <eol>
            getNextToken();
            definitionStatus = eol();
            
            // verification of: func <eol> ID 
            if (activeToken->tokenType != T_IDENTIFIER){ 
                DEBUG_PRINTF("[Parser] Leaving function definition() with %d ...\n",false);
                return false;
            }

            symtableInsert(symTable,activeToken->value->str,true);

            // verification of: func <eol> ID <eol>
            getNextToken();
            definitionStatus = definitionStatus && eol();

            // verification of: func <eol> ID <eol> (
            if (activeToken->tokenType != T_LEFT_BRACKET){ 
                DEBUG_PRINTF("[Parser] Leaving function definition() with %d ...\n",false);
                return false;
            }

            // verification of: func <eol> ID <eol> (<functionParams>
            getNextToken();
            definitionStatus = definitionStatus && functionParams();

            symtableFunctionEndOfArguments(symTable);

            // verification of: func <eol> ID <eol> (<functionParams>) <eol>
            definitionStatus = definitionStatus && eol();

            // verification of: func <eol> ID <eol> (<functionParams>) <eol> <funcDefMid>
            definitionStatus = definitionStatus && funcDefMid();
            symtableExitScope(symTable);
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function definition() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function definition() with %d ...\n",definitionStatus);
    return definitionStatus;
}

bool funcDefMid(){
    bool funcDefMidStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function funcDefMid()...\n");

    switch(activeToken->tokenType) {
        case T_LEFT_CURLY_BRACKET:
            // 12) <funcDefMid> -> {<statements>}
            getNextToken();
            funcDefMidStatus = statements();
            break;
        case T_ARROW:
            // 13) <funcDefMid> -> -> <eol> <type> <eol> {<statements>}
            getNextToken();
            funcDefMidStatus = eol() && type() && eol();

            // verification of: left curly bracket
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function funcDefMid() with %d ...\n",false);
                return false;
            }

            getNextToken();
            funcDefMidStatus = funcDefMidStatus && statements();
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function funcDefMid() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function funcDefMid() with %d ...\n",funcDefMidStatus);
    return funcDefMidStatus;
}

bool functionParams(){
    bool functionParamsStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function functionParams()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
            // 14) <functionParams> -> EPS
            getNextToken();
            functionParamsStatus = true;
            break;
        case T_EOL:
            // 15) <functionParams> -> EOL <functionParams>
            getNextToken();
            functionParamsStatus = functionParams();
            break;
        case T_IDENTIFIER:
        case KW_UNDERSCORE:
            // 16) <functionParams> -> <functionParam> <functionParamsN> <eol>
            functionParamsStatus = functionParam() && functionParamsN() && eol();
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function functionParams() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function functionParams() with %d ...\n",functionParamsStatus);
    return functionParamsStatus;
}

bool functionParamsN(){
    bool functionParamsNStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function functionParamsN()...\n");

    switch(activeToken->tokenType) {
        case T_COMMA:
            // 17) <functionParamsN> -> , <eol> <functionParam> <functionParamsN>
            getNextToken();
            functionParamsNStatus = eol() && functionParam() && functionParamsN();
            break;
        case T_RIGHT_BRACKET:
        case T_EOL:
            // 18) <functionParamsN> -> EPS
            getNextToken();
            functionParamsNStatus = true;
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function functionParamsN() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function functionParamsN() with %d ...\n",functionParamsNStatus);
    return functionParamsNStatus;
}

bool functionParam(){
    bool functionParamStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function functionParam()...\n");

    symtableAddFunctionNextArgument(symTable);

    switch(activeToken->tokenType) {
        case KW_UNDERSCORE:
            // 19) <functionParam> -> _ <eol> ID <eol> : <eol> <type> <eol>
            
            symtableSetFunctionArgumentName(symTable,"_");

            // verification of: _ <eol>
            getNextToken();
            functionParamStatus = eol();

            // verification of: _ <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d ...\n",false);
                return false;
            }

            symtableSetFunctionArgumentID(symTable,activeToken->value->str);

            // verification of: _ <eol> ID <eol>
            getNextToken();
            functionParamStatus = functionParamStatus && eol();

            // verification of: _ <eol> ID <eol> :
            if (activeToken->tokenType != T_COLON){
                DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d no colon...\n",false);
                return false;
            }

            // verification of: _ <eol> ID <eol> : <eol> <type> <eol>
            getNextToken();
            functionParamStatus = functionParamStatus && eol() && type() && eol();
            break;
        case T_IDENTIFIER:
            // 20) <functionParam> -> ID <eol> ID <eol> : <eol> <type> <eol>

            symtableSetFunctionArgumentName(symTable,activeToken->value->str);

            // verification of: ID <eol>
            getNextToken();
            functionParamStatus = eol();

            // verification of: ID <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d ...\n",false);
                return false;
            }

            symtableSetFunctionArgumentID(symTable,activeToken->value->str);

            // verification of: ID <eol> ID <eol>
            getNextToken();
            functionParamStatus = functionParamStatus && eol();

            // verification of: ID <eol> ID <eol> :
            if (activeToken->tokenType != T_COLON){
                DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d ...\n",false);
                return false;
            }
            // verification of: ID <eol> ID <eol> : <eol> <type> <eol>
            getNextToken();
            functionParamStatus = eol() && type() && eol();
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d ...\n",functionParamStatus);
    return functionParamStatus;
}

bool statements(){
    bool statementsStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function statements()...\n");

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
            statementsStatus = eol() && statementsBlock() && eol();
            break;
        case T_RIGHT_CURLY_BRACKET:
            // 23) <statements> -> EPS
            getNextToken();
            statementsStatus = true;
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function statements() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function statements() with %d ...\n",statementsStatus);
    return statementsStatus;
}

bool statementsBlock(){
    bool statementsBlockStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function statementsBlock()...\n");

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
            statementsBlockStatus = statement();

            // verification of: <statement> EOL
            if (typeOfLastToken != T_EOL && activeToken->tokenType != T_RIGHT_CURLY_BRACKET){ 
                if (activeToken->tokenType != T_EOL){ 
                    DEBUG_PRINTF("[Parser] Leaving function statementsBlock() with %d ...\n",false);
                    return false;
                }
                getNextToken();
            }

            statementsBlockStatus = statementsBlockStatus && statementsBlock();
            break;
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 25) <statementsBlock> -> EPS
            getNextToken();
            statementsBlockStatus = true;
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function statementsBlock() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function statementsBlock() with %d ...\n",statementsBlockStatus);
    return statementsBlockStatus;
}

bool statement(){
    bool statementStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function statement()...\n");

    switch(activeToken->tokenType) {
        case KW_LET:
        case KW_VAR:
            // 26) <statement> -> <varDec>
            statementStatus = varDec();
            break;
        case KW_IF:
            // 27) <statement> -> if <eol> <expression> <eol> {<statements>} <eol> else <eol> {<statements>}
            
            symtableEnterScope(symTable,"if",NULL);
            
            getNextToken();
            statementStatus = eol() && expression() && eol();

            //Generator
            symtablePushCode(symTable,concatString(5,"JUMPIFNEQ $",
                                                    symtableGetScopePrefixName(symTable),
                                                    "else ",
                                                    generatorPopFirstStringFromList(gen->parserStack),
                                                    " bool@true"));

            // verification of: if <eol>  <expression> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> 
            getNextToken();
            statementStatus = statementStatus && statements() && eol();

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> else
            if (activeToken->tokenType != KW_ELSE){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            //Generator
            symtablePushCode(symTable,concatString(3,"JUMP $",
                                                    symtableGetScopePrefixName(symTable),
                                                    "end"));
            symtablePushCode(symTable,concatString(3,"LABEL $",
                                                    symtableGetScopePrefixName(symTable),
                                                    "else"));

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> else <eol>
            getNextToken();
            statementStatus = statementStatus && eol();

            // verification of: if <eol>  <expression> <eol> {<statements>} <eol> else <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            getNextToken();
            statementStatus = statementStatus && statements();

            //Generator
            symtablePushCode(symTable,concatString(3,"LABEL $",
                                                    symtableGetScopePrefixName(symTable),
                                                    "end"));

            symtableExitScope(symTable);

            break;
        case KW_WHILE:
            // 28) <statement> -> while <eol> <expression> <eol> {<statements>}
            //TO DO generování kodu
            symtableEnterScope(symTable,"while",NULL);

            getNextToken();
            statementStatus = eol() && expression() && eol();

            // verification of: while <eol>  <expression> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            getNextToken();
            statementStatus = statementStatus && statements();

            symtableExitScope(symTable);

            break;
        case KW_RETURN:
            // 29) <statement> -> return <returnExpression>
            getNextToken();
            statementStatus = returnExpression();
            break;
        case T_IDENTIFIER:
            // 30) <statement> -> ID <callOrAssign>
            //TO DO symtable kontrola symtable jestli je var/func definovaná - setActive prvek
            symtableSetActiveItem(symTable,activeToken->value->str);
            symtableSetFunctionCallName(symTable,activeToken->value->str);

            //Generator
            generatorPushStringFirstToList(gen->parserStack,activeToken->value->str);

            getNextToken();
            statementStatus = callOrAssign();
            break;
        /*case KW_READSTRING:
            // 66) <statement> -> KW_VESTAVENA_FUNKC ()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = true;
            break;
        case KW_READINT:
            // 66) <statement> -> KW_VESTAVENA_FUNKC ()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = true;
            break;
        case KW_READDOUBLE:
            // 66) <statement> -> KW_VESTAVENA_FUNKC ()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = true;
            break;*/
        case KW_WRITE:
            // 66) <statement> -> write(<arguments>)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"write");

            statementStatus = arguments();

            //Generator
            while (listLength(gen->parserStack) != 0){
                symtablePushCode(symTable,concatString(2,"WRITE ",generatorPopLastStringFromList(gen->parserStack)));
            }
            break;
        /*case KW_INT_TO_DOUBLE:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = argument();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            break;
        case KW_DOUBLE_TO_INT:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = argument();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            break;
        case KW_LENGTH:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = argument();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            break;
        case KW_SUBSTRING:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>)
            // verification of: KW_VESTAVENA_FUNKCE (<argument>
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = argument();
            
            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = statementStatus && argument();

            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = statementStatus && argument();

            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>)
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            break;
        case KW_ORD:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = argument();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            break;
        case KW_CHR:
            // 66) <statement> -> KW_VESTAVENA_FUNKCE (<argument>)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            statementStatus = argument();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            getNextToken();
            break;*/
        default:
            DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
            return false;  
    }
    DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",statementStatus);
    return statementStatus;
}

bool callOrAssign(){
    bool callOrAssignStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function callOrAssign()...\n");

    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
        case T_INCREMENT:
        case T_DECREMENT:
        case T_VAR_MUL_VAR:
        case T_VAR_DIV_VAR:
        case T_EOL:
            // 36) <callOrAssign> -> <eol> <assign>>
            callOrAssignStatus = eol() && assign();
            break;
        case T_LEFT_BRACKET:
            // 37) <callOrAssign> -> (<arguments>)

            //Generator
            symtablePushCode(symTable,"CREATEFRAME");

            char *functionName = generatorPopFirstStringFromList(gen->parserStack);
            
            //Symtable
            symtableFunctionCallStart(symTable,NULL);

            getNextToken();
            callOrAssignStatus = arguments();

            int i = 1;
            char *result = concatString(1,"Toto zde musime nechat jinak to hodi segfault. Tuto poznamku muzete ingnorovat protoze se stejne prepise :)");
            while(listLength(gen->parserStack) != 0){
                snprintf(result, sizeof(result), "%d", i);
                symtablePushCode(symTable,concatString(2,"DEFVAR TF@!",result));
                symtablePushCode(symTable,concatString(4,"MOVE TF@!",result," ",generatorPopFirstStringFromList(gen->parserStack)));
                i++;
            }

            symtablePushCode(symTable,concatString(2,"CALL $",functionName));

            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function callOrAssign() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function callOrAssign() with %d ...\n",callOrAssignStatus);
    return callOrAssignStatus;
}

bool assign(){
    bool assignStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function assign()...\n");

    char *tempVarNameFromExpParser;
    char *destinationVarName;
    char *destinationVarNameWithPrefix;
    char *tempGeneratedName;
    char *tempNameWithPrefix;

    enum data_type lastVarType = symtableGetActiveItemType(symTable);
    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:
            // 61) <assign> -> = <expression>
            getNextToken();
            assignStatus = expression();
            symtableSameTypes(lastVarType,state->expParserReturnType);

            //Generator
            tempVarNameFromExpParser = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);

            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempVarNameFromExpParser));
            break;
        case T_INCREMENT:;
            // 62) <assign> -> += <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { 
                //TO DO tady to nefunguje
                raiseError(ERR_UNDEFINED_VARIABLE); 
            }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            getNextToken();
            assignStatus = expression();
            symtableSameTypes(lastVarType,state->expParserReturnType);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(6, "ADD ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        case T_DECREMENT:;
            // 63) <assign> -> -= <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { raiseError(ERR_UNDEFINED_VARIABLE); }

            if (lastVarType == DATA_TYPE_STRING ) { raiseError(ERR_WRONG_TYPE); }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            getNextToken();
            assignStatus = expression();
            symtableSameTypes(lastVarType,state->expParserReturnType);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(6, "SUB ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        case T_VAR_MUL_VAR:;
            // 64) <assign> -> *= <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { raiseError(ERR_UNDEFINED_VARIABLE); }

            if (lastVarType == DATA_TYPE_STRING ) { raiseError(ERR_WRONG_TYPE); }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            getNextToken();
            assignStatus = expression();
            symtableSameTypes(lastVarType,state->expParserReturnType);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(6, "MUL ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        case T_VAR_DIV_VAR:;
            // 65) <assign> -> /= <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { raiseError(ERR_UNDEFINED_VARIABLE); }

            //TO DO semantika dělení nulou řešíme my nebo ne??
            if (lastVarType == DATA_TYPE_STRING ) { raiseError(ERR_WRONG_TYPE); }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            getNextToken();
            assignStatus = expression();
            symtableSameTypes(lastVarType,state->expParserReturnType);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            if (lastVarType == T_INT){
                symtablePushCode(symTable,concatString(6, "IDIV ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            }
            else if (lastVarType == T_DOUBLE){
                symtablePushCode(symTable,concatString(6, "DIV ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            }
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function assign() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function assign() with %d ...\n",assignStatus);
    return assignStatus;
}

bool varDec(){
    bool varDecStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function varDec()...\n");

    switch(activeToken->tokenType) {
        case KW_LET:
            // 38) <varDec> -> let <eol> ID <eol> <varDecMid>

            getNextToken();
            varDecStatus = eol();

            // verification of: let <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                DEBUG_PRINTF("[Parser] Leaving function varDec() with %d ...\n",false);
                return false;
            }

            //Symtable
            symtableInsert(symTable,activeToken->value->str,false);

            //Generator
            symtablePushCode(symTable,concatString(3,"DEFVAR ", symtableGetVariablePrefix(symTable,activeToken->value->str),activeToken->value->str));
            generatorPushStringFirstToList(gen->parserStack,concatString(2, symtableGetVariablePrefix(symTable,activeToken->value->str),activeToken->value->str));

            getNextToken();
            varDecStatus = eol() && varDecMid();
            break;
        case KW_VAR:
            // 39) <varDec> -> var <eol> ID <eol> <varDecMid>

            getNextToken();
            varDecStatus = eol();
            
            // verification of: var <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                DEBUG_PRINTF("[Parser] Leaving function varDec() with %d ...\n",false);
                return false;
            }

            //Symtable
            symtableInsert(symTable,activeToken->value->str,false);
            
            //Generator
            symtablePushCode(symTable,concatString(3,"DEFVAR ",symtableGetVariablePrefix(symTable,activeToken->value->str),activeToken->value->str));
            generatorPushStringFirstToList(gen->parserStack,concatString(2,symtableGetVariablePrefix(symTable,activeToken->value->str),activeToken->value->str));

            getNextToken();
            varDecStatus = eol() && varDecMid();
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function varDec() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function varDec() with %d ...\n",varDecStatus);
    return varDecStatus;
}

bool varDecMid(){
    bool varDecMidStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function varDecMid()...\n");

    switch(activeToken->tokenType) {
        case T_COLON:
            // 40) <varDecMid> -> : <eol> <type> <eol> <varDef>
            getNextToken();
            varDecMidStatus = eol() && type() && eol() && varDef();
            break;
        case T_ASSIGNMENT:
            // 41) <varDecMid> -> = <expression>
            getNextToken();
            varDecMidStatus = expression();
            //TO DO umí expressionParser vracet nil?? -> nil vracet jako DATA_TYPE_NOTSET
            //TO DO if nill -> raise error 8
            if (state->expParserReturnType == DATA_TYPE_NOTSET) { raiseError(ERR_MISSING_TYPE); }
            symtableSetDataType(symTable,state->expParserReturnType,false);

            //Generator
            symtablePushCode(symTable,concatString(4,"MOVE ",generatorPopFirstStringFromList(gen->parserStack)," ",generatorPopFirstStringFromList(gen->parserStack)));
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function varDecMid() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function varDecMid() with %d ...\n",varDecMidStatus);
    return varDecMidStatus;
}

bool varDef(){
    bool varDefStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function varDef()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 42) <varDef> -> EPS
            generatorPopFirstStringFromList(gen->parserStack);
            varDefStatus = true;
            break;
        case T_ASSIGNMENT:
            // 43) <varDef> -> = <expression>
            getNextToken();

            varDefStatus = expression();
            symtableSameTypes(symtableGetActiveItemType(symTable),state->expParserReturnType);

            //Generator
            symtablePushCode(symTable,concatString(4,"MOVE ",generatorPopFirstStringFromList(gen->parserStack)," ",generatorPopFirstStringFromList(gen->parserStack)));
            break;
        default:
            // verification of: EOL
            if (typeOfLastToken == T_EOL){ 
                generatorPopFirstStringFromList(gen->parserStack);
                varDefStatus = true;
                break;
            }
            DEBUG_PRINTF("[Parser] Leaving function varDef() with %d ...\n",false);
            return false; 
    }
    DEBUG_PRINTF("[Parser] Leaving function varDef() with %d ...\n",varDefStatus);
    return varDefStatus;
}

bool returnExpression(){
    bool returnExpressionStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function returnExpression()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_CURLY_BRACKET:
        case T_EOL:
            // 45) <returnExpression> -> EPS
            if(symtableGetReturnTypeOfCurrentScope(symTable) != DATA_TYPE_VOID){ raiseError(ERR_WRONG_RETURN_TYPE); }
            returnExpressionStatus = true;
            break;
        default:
            // 44) <returnExpression> -> <expression>
            returnExpressionStatus = expression();
            if(symtableGetReturnTypeOfCurrentScope(symTable) == DATA_TYPE_VOID && state->expParserReturnType != DATA_TYPE_NOTSET){
                raiseError(ERR_WRONG_RETURN_TYPE);
            }
            if (symtableGetReturnTypeOfCurrentScope(symTable) != state->expParserReturnType) { 
                raiseError(ERR_WRONG_RETURN_TYPE); 
            }
            symtablePushCode(symTable,concatString(2,"MOVE LF@%retval ",generatorPopFirstStringFromList(gen->parserStack)));
            break;
    }
    DEBUG_PRINTF("[Parser] Leaving function returnExpression() with %d ...\n",returnExpressionStatus);
    return returnExpressionStatus;
}

bool arguments(){
    bool argumentsStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function arguments()...\n");

    switch(activeToken->tokenType) {
        case T_RIGHT_BRACKET:
            // 46) <arguments> -> EPS
            getNextToken();
            argumentsStatus = true;
            break;
        case T_EOL:
            // 47) <arguments> -> EOL <arguments>
            getNextToken();
            argumentsStatus = arguments();
            break;
        case T_IDENTIFIER:
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 48) <arguments> -> <argument> <argumentsN> <eol>
            argumentsStatus = argument() && argumentsN(); //&& eol()
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function arguments() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function arguments() with %d ...\n",argumentsStatus);
    return argumentsStatus;
}

bool argumentsN(){
    bool argumentsNStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function argumentsN()...\n");

    switch(activeToken->tokenType) {
        case T_COMMA:
            // 49) <argumentsN> -> , <eol> <argument> <argumentsN>
            getNextToken();
            argumentsNStatus = eol() && argument() && argumentsN();
            break;
        case T_RIGHT_BRACKET:
            // 50) <argumentsN> -> EPS
            getNextToken();
            argumentsNStatus = true;
            break;
        case T_EOL:
            // 50) <argumentsN> -> EPS
            getNextToken();
            argumentsNStatus = argumentsN();
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function argumentsN() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function argumentsN() with %d ...\n",argumentsNStatus);
    return argumentsNStatus;
}

bool argument(){
    bool argumentStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function argument()...\n");

    symtableFunctionCallNextParameter(symTable);
    switch(activeToken->tokenType) {
        //TO DO změnit první case na default jestli to něco nerozbije
        case T_INT:
        case T_DOUBLE:
        case T_STRING:
            // 51) <argument> -> <expression>
            argumentStatus = expression();

            //Symtable
            if (state->expParserReturnType == DATA_TYPE_NOTSET) { 
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,true); 
            }
            else {
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,false);
            }
            break;
        case T_IDENTIFIER:;
            // 52) <argument> -> ID <eol> <argWithName>
            token *tempToken = tokenInit();
            tempToken->tokenType = activeToken->tokenType;
            int stringLength = strlen(activeToken->value->str) + 1;
            char *string = (char *)malloc(stringLength);
            memcpy(string,activeToken->value->str,stringLength);
            tempToken->value->str = string;

            getNextToken();
            argumentStatus = eol();
            listPushBack(state->tokenQueue,tempToken);
            argumentStatus = argumentStatus && argWithName();
            tokenFree(tempToken);
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function argument() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function argument() with %d ...\n",argumentStatus);
    return argumentStatus;
}

bool argWithName(){
    bool argWithNameStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function argWithName()...\n");

    switch(activeToken->tokenType) {
        case T_COLON:
            // 53) <argWithName> -> : <eol> <expression>

            //Symtable
            activeToken = listPopLast(state->tokenQueue);
            symtableFunctionCallSetParameterName(symTable,activeToken->value->str);
            getNextToken();
            argWithNameStatus = eol() && expression();

            //Symtable
            if (state->expParserReturnType == DATA_TYPE_NOTSET) { 
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,true); 
            }
            else {
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,false);
            }
            break;
        case T_RIGHT_BRACKET:
        case T_COMMA:
        case T_EOL:
            // 54) <argWithName> -> EPS
            argWithNameStatus = expressionParserStart(state);

            //Symtable
            if (state->expParserReturnType == DATA_TYPE_NOTSET) { 
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,true); 
            }
            else {
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,false);
            }
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function argWithName() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function argWithName() with %d ...\n",argWithNameStatus);
    return argWithNameStatus;
}

bool expression(){
    bool expressionStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function expression()...\n");

    char *tempNameWithPrefix;
    char *tempGeneratedName;

    switch(activeToken->tokenType) {
        case KW_READSTRING:
            // 67) <expression> -> readString()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"READ ",tempNameWithPrefix, " string"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_STRING;

            getNextToken();
            expressionStatus = true;
            break;
        case KW_READINT:
            // 67) <expression> -> readInt()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"READ ",tempNameWithPrefix, " int"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_INT;

            getNextToken();
            expressionStatus = true;
            break;
        case KW_READDOUBLE:
            // 67) <expression> -> readDouble()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"READ ",tempNameWithPrefix, " float"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_DOUBLE;

            getNextToken();
            expressionStatus = true;
            break;
        case KW_INT_TO_DOUBLE:
            // 67) <expression> -> Int2Double(term : Int)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            //symtableFunctionCallStart(symTable,"Int2Double");

            expressionStatus = argument();

            if(state->expParserReturnType != T_INT){raiseError(ERR_WRONG_TYPE);}

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "INT2FLOAT ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            state->expParserReturnType = T_DOUBLE;

            getNextToken();
            break;
        case KW_DOUBLE_TO_INT:
            // 67) <expression> -> Double2Int(term : Double)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            //symtableFunctionCallStart(symTable,"Double2Int");

            expressionStatus = argument();

            if(state->expParserReturnType != T_DOUBLE){raiseError(ERR_WRONG_TYPE);}

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "FLOAT2INT ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            state->expParserReturnType = T_INT;

            getNextToken();
            break;
        case KW_LENGTH:
            // 67) <expression> -> length(s : String)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            //TO DO má tady být ten call start
            //symtableFunctionCallStart(symTable,"length");

            expressionStatus = argument();

            if(state->expParserReturnType != T_STRING){raiseError(ERR_WRONG_TYPE);}

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "STRLEN ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);
            
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            state->expParserReturnType = T_INT;

            getNextToken();
            break;
        case KW_SUBSTRING:
            // 67) <expression> -> substring(s : String, start : Int, end : Int)
            // 67) <expression> -> substring(<argument>,<argument>,<argument>)
            // verification of: KW_VESTAVENA_FUNKCE (<argument>
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            //symtableFunctionCallStart(symTable,"substring");

            expressionStatus = argument();

            if(state->expParserReturnType != T_INT){raiseError(ERR_WRONG_TYPE);}
            
            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();
            expressionStatus = expressionStatus && argument();

            if(state->expParserReturnType != T_INT){raiseError(ERR_WRONG_TYPE);}

            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();
            expressionStatus = expressionStatus && argument();
            
            if(state->expParserReturnType != T_INT){raiseError(ERR_WRONG_TYPE);}

            // verification of: KW_VESTAVENA_FUNKCE (<argument>,<argument>,<argument>)
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            //Generator
            //TO DO

            //TO DO dodělat že je to String?
            state->expParserReturnType = T_STRING;

            getNextToken();
            break;
        case KW_ORD:
            // 67) <expression> -> ord(s : String)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            //symtableFunctionCallStart(symTable,"ord");

            expressionStatus = argument();

            if(state->expParserReturnType != T_STRING){raiseError(ERR_WRONG_TYPE);}

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            //Generator
            // TO DO

            state->expParserReturnType = T_INT;

            getNextToken();
            break;
        case KW_CHR:
            // 67) <expression> -> chr(i : Int)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            //symtableFunctionCallStart(symTable,"chr");

            expressionStatus = argument();

            if(state->expParserReturnType != T_INT){raiseError(ERR_WRONG_TYPE);}

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",false);
                return false;
            }

            //Generator
            // TO DO

            state->expParserReturnType = T_STRING;

            getNextToken();
            break;
        default:
            //TO DO Naučit parser poznat kw vestavene funkce a on mi potom při nich a při voláno normálních funkcí bude volat jednu určitou moji funkci
            listPushBack(state->tokenQueue,activeToken);
            expressionStatus = expressionParserStart(state); 
            getNextToken();
            break;
    }
    DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",expressionStatus);
    return expressionStatus;
}
