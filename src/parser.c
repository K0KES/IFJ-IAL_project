/// @file parser.c
/// @author Lukáš Prokeš
/// @brief Parser 
/// @date 30.11.2023

#include "parser.h"

enum tokenType typeOfLastToken;

bool isLetId = false;

symtable *symTable;
generator *gen;
token *activeToken;
programState *state;

token *tokenInit(){
    token *activeToken = (token*)malloc(sizeof(token));
    if(activeToken == NULL){ raiseError(ERR_INTERNAL); }

    activeToken->value = strInit();

    activeToken->position = (positionInfo*)malloc(sizeof(positionInfo));
    if(activeToken->position == NULL){ raiseError(ERR_INTERNAL); }

    activeToken->position->charNumber = 0;
    activeToken->position->lineNumber = 1;
    activeToken->is_nullable = false;
    activeToken->is_return_from_func = false;
    activeToken->lastChar = '\0';
    activeToken->tokenType = T_NULLABLE;
    activeToken->tokenExpParserType = T_NULLABLE;
    activeToken->is_number_literal = false;
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
            
        case T_EOL:
            // 5) <code> -> EOF
            getNextToken();
            codeStatus = code();
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
        default:
            // 7) <eol> -> EPS
            eolStatus = true;
            break;
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
                symtableSetVariableValue(symTable);
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
                symtableSetVariableValue(symTable);
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
                symtableSetVariableValue(symTable);
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

            symtableInsert(symTable,strGetStr(activeToken->value),true);

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

            symtableSetFunctionArgumentID(symTable,strGetStr(activeToken->value));

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

            symtableSetFunctionArgumentName(symTable,strGetStr(activeToken->value));

            // verification of: ID <eol>
            getNextToken();
            functionParamStatus = eol();

            // verification of: ID <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                DEBUG_PRINTF("[Parser] Leaving function functionParam() with %d ...\n",false);
                return false;
            }

            symtableSetFunctionArgumentID(symTable,strGetStr(activeToken->value));

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

    char *letIdName;

    switch(activeToken->tokenType) {
        case KW_LET:
        case KW_VAR:
            // 26) <statement> -> <varDec>
            statementStatus = varDec();
            break;
        case KW_IF:
            // 27) <statement> -> if <eol> <letExp> <eol> {<statements>} <eol> else <eol> {<statements>}
            
            symtableEnterScope(symTable,"&if",NULL);
            
            getNextToken();
            symtablePushCode(symTable,"");
            symtablePushCode(symTable,"#Start of IF statement");
            statementStatus = eol() && letExp() && eol();

            if (isLetId){ letIdName = generatorPopFirstStringFromList(gen->parserStack); }

            //Generator
            symtablePushCode(symTable,concatString(5,"JUMPIFNEQ $",
                                                    symtableGetScopePrefixName(symTable),
                                                    "else ",
                                                    generatorPopFirstStringFromList(gen->parserStack),
                                                    " bool@true"));

            char *labelIfScopePrefixName = allocateString(symtableGetScopePrefixName(symTable));

            // verification of: if <eol>  <letExp> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            // verification of: if <eol>  <letExp> <eol> {<statements>} <eol> 
            getNextToken();
            statementStatus = statementStatus && statements() && eol();
            
            if (isLetId){ 
                symtableSetVariableNullable(symTable,letIdName,true); 
                symtableSetActiveItem(symTable,letIdName);
                symtableVariableIsNotConstant(symTable);
                isLetId = false;  
            }
            symtableExitScope(symTable);

            // verification of: if <eol>  <letExp> <eol> {<statements>} <eol> else
            if (activeToken->tokenType != KW_ELSE){
                DEBUG_PRINTF("[Parser] Error - missing else\n");
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }
            symtableEnterScope(symTable,"&if",NULL);

            //Generator
            symtablePushCode(symTable,concatString(3,"JUMP $",
                                                    labelIfScopePrefixName,
                                                    "end"));
            symtablePushCode(symTable,concatString(3,"LABEL $",
                                                    labelIfScopePrefixName,
                                                    "else"));

            // verification of: if <eol>  <letExp> <eol> {<statements>} <eol> else <eol>
            getNextToken();
            statementStatus = statementStatus && eol();

            // verification of: if <eol>  <letExp> <eol> {<statements>} <eol> else <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            getNextToken();
            statementStatus = statementStatus && statements();

            //Generator
            symtablePushCode(symTable,concatString(3,"LABEL $",
                                                    labelIfScopePrefixName,
                                                    "end"));

            symtablePushCode(symTable,"#End of IF statement");
            symtableExitScope(symTable);

            break;
        case KW_WHILE:
            // 28) <statement> -> while <eol> <letExp> <eol> {<statements>}
            symtableEnterScope(symTable,"&while",NULL);

            symtablePushCode(symTable,"");
            symtablePushCode(symTable,"#Start of while");
            symtablePushCode(symTable,concatString(2,"LABEL $",symtableGetScopePrefixName(symTable)));

            getNextToken();
            state->changeToDouble = false;
            statementStatus = eol() && letExp() && eol();

            if (isLetId){ letIdName = generatorPopFirstStringFromList(gen->parserStack); }

            //Generator
            symtablePushCode(symTable,concatString(5,"JUMPIFNEQ $",
                                                    symtableGetScopePrefixName(symTable),
                                                    "end ",
                                                    generatorPopFirstStringFromList(gen->parserStack),
                                                    " bool@true"));
            symtablePushCode(symTable,"#While body");

            // verification of: while <eol>  <letExp> <eol> {
            if (activeToken->tokenType != T_LEFT_CURLY_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
                return false;
            }

            getNextToken();
            statementStatus = statementStatus && statements();
            symtablePushCode(symTable,concatString(2,"JUMP $",symtableGetScopePrefixName(symTable)));
            symtablePushCode(symTable,concatString(3,"LABEL $",symtableGetScopePrefixName(symTable),"end"));
            symtablePushCode(symTable,"#End of while");

            if (isLetId){ 
                symtableSetVariableNullable(symTable,letIdName,true); 
                symtableSetActiveItem(symTable,letIdName);
                symtableVariableIsNotConstant(symTable);
                isLetId = false;  
            }
            symtableExitScope(symTable);

            break;
        case KW_RETURN:
            // 29) <statement> -> return <returnExpression>
            
            //TO DO přidat do symtable informaci že byl zavolán return a potom kontrolovat jestli byl všude kde měl být
            if(symTable->currentFunction == NULL){
                DEBUG_PRINTF("[Parser] Error call return out of function\n");
                raiseError(ERR_SYNTAX);
            }
            getNextToken();
            statementStatus = returnExpression() && eol();
            symtableFunctionReturnWasCalled(symTable);
            break;
        case T_IDENTIFIER:
            // 30) <statement> -> ID <callOrAssign>
            symtableSetActiveItem(symTable,strGetStr(activeToken->value));
            symtableSetFunctionCallName(symTable,strGetStr(activeToken->value));

            //Generator
            generatorPushStringFirstToList(gen->parserStack,strGetStr(activeToken->value));

            getNextToken();
            statementStatus = callOrAssign();
            break;
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
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_VOID,false);

            statementStatus = arguments();

            //Generator
            int i = 0;
            while (i < symtableFunctionCallGetNumberOfParameters(symTable)){
                symtablePushCode(symTable,concatString(2,"WRITE ",generatorPopLastStringFromList(gen->parserStack)));
                i++;
            }
            symtableFunctionCallEnd(symTable);
            break;
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
            // 66) <statement> -> <parseBuildInFunctions>

            statementStatus = parseBuidInFunctions();

            generatorPopFirstStringFromList(gen->parserStack);
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",false);
            return false;  
    }
    DEBUG_PRINTF("[Parser] Leaving function statement() with %d ...\n",statementStatus);
    return statementStatus;
}

bool letExp(){
    bool letExpStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function letExp()...\n");

    switch(activeToken->tokenType) {
        case KW_LET:
            // 68) <letExp> -> let <eol> ID
            getNextToken();

            letExpStatus = eol();

            // verification of: let <eol> ID
            if (activeToken->tokenType != T_IDENTIFIER){
                DEBUG_PRINTF("[Parser] Leaving function letExp() with %d ...\n",false);
                return false;
            }

            if (symtableGetVariableNullable(symTable,strGetStr(activeToken->value)) == false){ raiseError(ERR_SEMANTIC);}
            isLetId = true;
            symtableSetVariableNullable(symTable,strGetStr(activeToken->value),false);
            symtableSetActiveItem(symTable,strGetStr(activeToken->value));
            symtableVariableIsConstant(symTable);
            symtableSetEndOfVariableDefinition(symTable);
            
            char *tempGeneratedName = generatorGenerateTempVarName(gen);
            char *tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(5,"EQ ", tempNameWithPrefix, " nil@nil ",symtableGetVariablePrefix(symTable,strGetStr(activeToken->value)),strGetStr(activeToken->value)));
            symtablePushCode(symTable,concatString(4,"NOT ", tempNameWithPrefix, " ", tempNameWithPrefix));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            generatorPushStringFirstToList(gen->parserStack,strGetStr(activeToken->value));
            getNextToken();
            break;
        default:
            // 69) <letExp> -> <expression>
            state->changeToDouble = false;
            letExpStatus = expression();
            if (state->expParserReturnType != DATA_TYPE_BOOL){ raiseError(ERR_WRONG_TYPE); }
            break;
    }
    DEBUG_PRINTF("[Parser] Leaving function letExp() with %d ...\n",letExpStatus);
    return letExpStatus;
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
        case T_LEFT_BRACKET:;
            // 37) <callOrAssign> -> (<arguments>)

            //Generator

            char *functionName = generatorPopFirstStringFromList(gen->parserStack);
            state->expParserReturnType = symtableGetVariableType(symTable,functionName);
            state->changeToDouble = false;
            
            //Symtable
            symtableFunctionCallStart(symTable,NULL);

            getNextToken();
            callOrAssignStatus = arguments();

            int i = symtableFunctionCallGetNumberOfParameters(symTable);
            char *result = allocateString("Toto zde musime nechat jinak to hodi segfault. Tuto poznamku muzete ingnorovat protoze se stejne prepise :)");
            symtablePushCodeCreateFrame(symTable);
            while(i > 0){
                snprintf(result, sizeof(result), "%d", i);
                symtablePushCode(symTable,concatString(2,"DEFVAR TF@!",result));
                symtablePushCode(symTable,concatString(4,"MOVE TF@!",result," ",generatorPopFirstStringFromList(gen->parserStack)));
                i--;
            }
            
            
            symtablePushCode(symTable,symTable->lastFunctionCall);
            char *tempGeneratedName = generatorGenerateTempVarName(gen);
            char *tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(3,"MOVE ",tempNameWithPrefix," TF@%retval"));
            //Návratová hodnota této funkce se stejně nikam neukládá
            //generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);
            symtableFunctionCallEnd(symTable);
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

    int assignTokenLastChar;

    enum data_type lastVarType = symtableGetActiveItemType(symTable);
    bool lastVarTypeNullable = symtableGetVariableNullable(symTable,symtableGetActiveItemName(symTable));
    switch(activeToken->tokenType) {
        case T_ASSIGNMENT:;
            // 61) <assign> -> = <expression>
            assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            if (lastVarType == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;} 

            symTable->inExpression = true;
            assignStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (lastVarTypeNullable == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(lastVarType,state->expParserReturnType);
            }
            if (state->expParserReturnTypeNullable){ 
                if (lastVarTypeNullable == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }

            //Generator
            tempVarNameFromExpParser = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);

            if(!symtableIsVariableDefined(symTable,destinationVarName)){
                DEBUG_PRINTF("[Parser] Error undefined variable %s\n",tempVarNameFromExpParser);
                raiseError(ERR_UNDEFINED_VARIABLE);
            }
            symtableSetVariableValue(symTable);

            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempVarNameFromExpParser));
            break;
        case T_INCREMENT:;
            // 62) <assign> -> += <expression>
            
            if (!symtableIsActiveVariableInitiated(symTable)) { 
                DEBUG_PRINTF("[Parser] Error variable is not initiated\n");
                raiseError(ERR_UNDEFINED_VARIABLE); 
            }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            
            if (lastVarType == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;} 

            symTable->inExpression = true;
            assignStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (lastVarTypeNullable == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(lastVarType,state->expParserReturnType);
            }     
            if (state->expParserReturnTypeNullable){ 
                if (lastVarTypeNullable == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }

            symtableSetVariableValue(symTable);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(6, "ADD ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        case T_DECREMENT:;
            // 63) <assign> -> -= <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { 
                DEBUG_PRINTF("[Parser] Error variable is not initiated\n");
                raiseError(ERR_UNDEFINED_VARIABLE); 
            }

            if (lastVarType == DATA_TYPE_STRING ) { 
                DEBUG_PRINTF("[Parser] Error cant decrement string with string\n");
                raiseError(ERR_WRONG_TYPE); 
            }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }

            if (lastVarType == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;} 

            symTable->inExpression = true;
            assignStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (lastVarTypeNullable == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(lastVarType,state->expParserReturnType);
            }     
            if (state->expParserReturnTypeNullable){ 
                if (lastVarTypeNullable == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }

            symtableSetVariableValue(symTable);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(6, "SUB ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        case T_VAR_MUL_VAR:;
            // 64) <assign> -> *= <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { 
                DEBUG_PRINTF("[Parser] Error variable is not initiated\n");
                raiseError(ERR_UNDEFINED_VARIABLE); 
            }

            if (lastVarType == DATA_TYPE_STRING ) { 
                 DEBUG_PRINTF("[Parser] Error cant multiply string with string\n");
                raiseError(ERR_WRONG_TYPE); 
            }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }

            if (lastVarType == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;} 

            symTable->inExpression = true;
            assignStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (lastVarTypeNullable == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(lastVarType,state->expParserReturnType);
            }     
            if (state->expParserReturnTypeNullable){ 
                if (lastVarTypeNullable == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }

            symtableSetVariableValue(symTable);

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(6, "MUL ", tempNameWithPrefix, " ", destinationVarNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(4, "MOVE ", destinationVarNameWithPrefix, " ", tempNameWithPrefix));
            break;
        case T_VAR_DIV_VAR:;
            // 65) <assign> -> /= <expression>
            if (!symtableIsActiveVariableInitiated(symTable)) { 
                DEBUG_PRINTF("[Parser] Error variable is not initiated\n");
                raiseError(ERR_UNDEFINED_VARIABLE); 
            }

            if (lastVarType == DATA_TYPE_STRING ) { 
                DEBUG_PRINTF("[Parser] Error cant divide string with string\n");
                raiseError(ERR_WRONG_TYPE); 
            }

            //Generator
            destinationVarName = generatorPopFirstStringFromList(gen->parserStack);
            destinationVarNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,destinationVarName),destinationVarName);

            //Parser
            assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            
            if (lastVarType == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;} 

            symTable->inExpression = true;
            assignStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (lastVarTypeNullable == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(lastVarType,state->expParserReturnType);
            }       
            if (state->expParserReturnTypeNullable){ 
                if (lastVarTypeNullable == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }

            symtableSetVariableValue(symTable);

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
            symtableInsert(symTable,strGetStr(activeToken->value),false);
            symtableVariableIsConstant(symTable);

            //Generator
            symtablePushCode(symTable,concatString(3,"DEFVAR ", symtableGetVariablePrefix(symTable,strGetStr(activeToken->value)),strGetStr(activeToken->value)));
            generatorPushStringFirstToList(gen->parserStack,concatString(2, symtableGetVariablePrefix(symTable,strGetStr(activeToken->value)),strGetStr(activeToken->value)));

            getNextToken();
            varDecStatus = eol() && varDecMid();
            symtableSetEndOfVariableDefinition(symTable);
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
            symtableInsert(symTable,strGetStr(activeToken->value),false);
            
            //Generator
            symtablePushCode(symTable,concatString(3,"DEFVAR ",symtableGetVariablePrefix(symTable,strGetStr(activeToken->value)),strGetStr(activeToken->value)));
            generatorPushStringFirstToList(gen->parserStack,concatString(2,symtableGetVariablePrefix(symTable,strGetStr(activeToken->value)),strGetStr(activeToken->value)));

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
            varDecMidStatus = eol() && type(); 
            
            if (symtableGetVariableNullable(symTable,symtableGetActiveItemName(symTable))){
                symtablePushCode(symTable,concatString(3,"MOVE ",generatorGetFirstStringFromList(gen->parserStack)," nil@nil"));
            }

            varDecMidStatus = varDecMidStatus && eol() && varDef();
            break;
        case T_ASSIGNMENT:;
            // 41) <varDecMid> -> = <expression>
            int assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }

            symTable->inExpression = true;
            varDecMidStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL) { raiseError(ERR_MISSING_TYPE); }
            symtableSetDataType(symTable,state->expParserReturnType,state->expParserReturnTypeNullable);
            symtableSetVariableValue(symTable);

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
        case T_ASSIGNMENT:;
            // 43) <varDef> -> = <expression>
            int assignTokenLastChar = activeToken->lastChar;
            getNextToken();

            // there is NOT space in front of operator
            if (assignTokenLastChar == 0 && typeOfLastToken != T_EOL){
                // next token is EOL
                if (activeToken->tokenType == T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
                // next token  has space in front of him
                if (activeToken->lastChar == 32){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }
            // there is space in front of operator
            else if (assignTokenLastChar == 32 || typeOfLastToken == T_EOL){
                if (activeToken->lastChar == 0 && activeToken->tokenType != T_EOL){
                    DEBUG_PRINTF("[Parser] Error wrong white space \n");
                    raiseError(ERR_SYNTAX);
                }
            }

            if (symtableGetActiveItemType(symTable) == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;} 

            symTable->inExpression = true;
            varDefStatus = expression();
            symTable->inExpression = false;

            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (symtableGetVariableNullable(symTable,symtableGetActiveItemName(symTable)) == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(symtableGetActiveItemType(symTable),state->expParserReturnType);
            }     
            if (state->expParserReturnTypeNullable){ 
                if ( symtableGetVariableNullable(symTable,symtableGetActiveItemName(symTable)) == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }

            symtableSetVariableValue(symTable);

            //Generator
            symtablePushCode(symTable,concatString(4,"MOVE ",generatorPopFirstStringFromList(gen->parserStack)," ",generatorPopFirstStringFromList(gen->parserStack)));
            break;
        case T_EOF:
            generatorPopFirstStringFromList(gen->parserStack);
            varDefStatus = true;
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
            if(symtableGetReturnTypeOfCurrentScope(symTable) != DATA_TYPE_VOID){
                DEBUG_PRINTF("[Parser] Error function should return value\n");
                raiseError(ERR_WRONG_RETURN_TYPE); 
            }
            symtablePushCode(symTable,concatString(3,"JUMP $",symTable->currentFunction->name,"_end"));
            returnExpressionStatus = true;
            break;
        default:
            // 44) <returnExpression> -> <expression>

            if (symtableGetReturnTypeOfCurrentScope(symTable) == DATA_TYPE_DOUBLE){ state->changeToDouble = true; }
            else{state->changeToDouble = false;}  

            returnExpressionStatus = expression();

            /*
            TO DO chceck if return type is nullable
            if (state->expParserReturnType == DATA_TYPE_NIL){
                if (lastVarTypeNullable == false){
                    raiseError(ERR_WRONG_TYPE);
                }
            }else{
                symtableCheckSameTypes(lastVarType,state->expParserReturnType);
            }   
            if (state->expParserReturnTypeNullable){ 
                if ( symtableGetVariableNullable(symTable,symtableGetActiveItemName(symTable)) == false ){
                    raiseError(ERR_WRONG_TYPE); 
                }
            }  
            */
            //TO DO co vrací exp parser v druhém případe následující podmínky -> zkontrolovat typy
            if(symtableGetReturnTypeOfCurrentScope(symTable) == DATA_TYPE_VOID && state->expParserReturnType != DATA_TYPE_NOTSET){
                DEBUG_PRINTF("[Parser] Error function should return void\n");
                raiseError(ERR_WRONG_RETURN_TYPE);
            }
            if (symtableGetReturnTypeOfCurrentScope(symTable) != state->expParserReturnType) { 
                DEBUG_PRINTF("[Parser] Error function wrong return type\n");
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS); 
            }
            symtablePushCode(symTable,concatString(2,"MOVE LF@%retval ",generatorPopFirstStringFromList(gen->parserStack)));
            symtablePushCode(symTable,concatString(3,"JUMP $",symTable->currentFunction->name,"_end"));
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
        case KW_READSTRING:
        case KW_READINT:
        case KW_READDOUBLE:
        case KW_INT_TO_DOUBLE:
        case KW_DOUBLE_TO_INT:
        case KW_LENGTH:
        case KW_SUBSTRING:
        case KW_ORD:
        case KW_CHR:
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
        case T_IDENTIFIER:;
            // 52) <argument> -> ID <eol> <argWithName>
            token *tempToken = tokenInit();
            tempToken->tokenType = activeToken->tokenType;
            tempToken->lastChar = activeToken->lastChar;

            char *string = allocateString(strGetStr(activeToken->value));
            strSetString(tempToken->value,string);

            getNextToken();
            argumentStatus = eol();
            DEBUG_PRINTF("[Parser] Pushing token %s to tokenQueue\n",getTokenName(tempToken->tokenType));
            listPushBack(state->tokenQueue,tempToken);
            argumentStatus = argumentStatus && argWithName();
            tokenFree(tempToken);
            break;
        default:
            // 51) <argument> -> <expression>
            argumentStatus = expression();

            //Symtable
            symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,state->expParserReturnTypeNullable); 

            // TO DO zpětné přetypování argumentu z int na double
            break;
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
            symtableFunctionCallSetParameterName(symTable,strGetStr(activeToken->value));
            getNextToken();
            argWithNameStatus = eol() && expression();

            //Symtable
            symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,state->expParserReturnTypeNullable); 
   
            break;
        default:;
            // 54) <argWithName> -> EPS
            token *tempToken = tokenInit();
            tempToken->tokenType = activeToken->tokenType;
            tempToken->lastChar = activeToken->lastChar;
            char *string = allocateString(strGetStr(activeToken->value));
            strSetString(tempToken->value,string);

            DEBUG_PRINTF("[Parser] Pushing token %s to tokenQueue\n",getTokenName(tempToken->tokenType));
            listPushBack(state->tokenQueue,tempToken);
            state->changeToDouble = false;
            argWithNameStatus = expressionParserStart(state);
            //Symtable
            //TO DO kontrola jaký typ vrací exp parser když je "non set"/nil/??
            DEBUG_PRINTF("Typ parametru je: %d\n",state->expParserReturnType);
            if (state->expParserReturnType == DATA_TYPE_NOTSET) { 
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,true); 
            }
            else {
                symtableFunctionCallSetParameterType(symTable,state->expParserReturnType,false);
            }
            getNextToken();
            break;
    }
    DEBUG_PRINTF("[Parser] Leaving function argWithName() with %d ...\n",argWithNameStatus);
    return argWithNameStatus;
}

bool expression(){
    bool expressionStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function expression()...\n");

    token *tempToken = tokenInit();
    tempToken->tokenType = activeToken->tokenType;
    tempToken->lastChar = activeToken->lastChar;
    char *string = allocateString(strGetStr(activeToken->value));
    strSetString(tempToken->value,string);

    DEBUG_PRINTF("[Parser] Pushing token %s to tokenQueue\n",getTokenName(tempToken->tokenType));
    listPushBack(state->tokenQueue,tempToken);
    expressionStatus = expressionParserStart(state); 
    getNextToken();
    
    DEBUG_PRINTF("[Parser] Leaving function expression() with %d ...\n",expressionStatus);
    return expressionStatus;
}

bool parseBuidInFunctions(){
    bool parseBuidInFunctionsStatus = false;
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function parseBuidInFunctions()...\n");

    char *tempNameWithPrefix;
    char *tempGeneratedName;

    switch(activeToken->tokenType) {
        case KW_READSTRING:
            // 67) <parseBuidInFunctions> -> readString()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                    DEBUG_PRINTF("[Parser] Error function dont need parameters\n");
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false; 
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"READ ",tempNameWithPrefix, " string"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_STRING;
            state->expParserReturnTypeNullable = true;
            state->changeToDouble = false;

            getNextToken();
            parseBuidInFunctionsStatus = true;
            break;
        case KW_READINT:
            // 67) <parseBuidInFunctions> -> readInt()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                    DEBUG_PRINTF("[Parser] Error function dont need parameters\n");
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false; 
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"READ ",tempNameWithPrefix, " int"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_INT;
            state->expParserReturnTypeNullable = true;
            state->changeToDouble = false;

            getNextToken();
            parseBuidInFunctionsStatus = true;
            break;
        case KW_READDOUBLE:
            // 67) <parseBuidInFunctions> -> readDouble()
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                    DEBUG_PRINTF("[Parser] Error function dont need parameters\n");
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false; 
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"READ ",tempNameWithPrefix, " float"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_DOUBLE;
            state->expParserReturnTypeNullable = true;
            state->changeToDouble = false;

            getNextToken();
            parseBuidInFunctionsStatus = true;
            break;
        case KW_INT_TO_DOUBLE:
            // 67) <parseBuidInFunctions> -> Int2Double(term : Int)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"Int2Double");
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_DOUBLE,false);

            parseBuidInFunctionsStatus = argument();

            if(state->expParserReturnType != T_INT){
                DEBUG_PRINTF("[Parser] Error function parameter should be int\n");
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "INT2FLOAT ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_COMMA){
                    getNextToken();
                    if(activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                        DEBUG_PRINTF("[Parser] Error function should have only one parameter\n");
                        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                    }
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }

            state->expParserReturnType = T_DOUBLE;
            state->expParserReturnTypeNullable = false;
            state->changeToDouble = false;

            getNextToken();

            symtableFunctionCallEnd(symTable);
            break;
        case KW_DOUBLE_TO_INT:
            // 67) <parseBuidInFunctions> -> Double2Int(term : Double)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"Double2Int");
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_INTEGER,false);

            parseBuidInFunctionsStatus = argument();

            if(state->expParserReturnType != T_DOUBLE){
                DEBUG_PRINTF("[Parser] Error function parameter should be double\n");
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "FLOAT2INT ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_COMMA){
                    getNextToken();
                    if(activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                        DEBUG_PRINTF("[Parser] Error function should have only one parameter\n");
                        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                    }
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }

            state->expParserReturnType = T_INT;
            state->expParserReturnTypeNullable = false;
            state->changeToDouble = false;

            getNextToken();

            symtableFunctionCallEnd(symTable);
            break;
        case KW_LENGTH:
            // 67) <parseBuidInFunctions> -> length(s : String)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"length");
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_INTEGER,false);

            parseBuidInFunctionsStatus = argument();

            if(state->expParserReturnType != T_STRING){
                DEBUG_PRINTF("[Parser] Error function parameter should be string\n");
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "STRLEN ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);
            
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_COMMA){
                    getNextToken();
                    if(activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                        DEBUG_PRINTF("[Parser] Error function should have only one parameter\n");
                        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                    }
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }

            state->expParserReturnType = T_INT;
            state->expParserReturnTypeNullable = false;
            state->changeToDouble = false;

            getNextToken();

            symtableFunctionCallEnd(symTable);
            break;
        case KW_SUBSTRING:
            // 67) <parseBuidInFunctions> -> substring(s : String, start : Int, end : Int)
            // verification of: substring(<argument>
            generatorAddSubstringFunction(gen);
            symtablePushCodeCreateFrame(symTable);
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"substring");
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_STRING,true);

            if (activeToken->tokenType != T_IDENTIFIER){
                if (activeToken->tokenType == T_RIGHT_BRACKET || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            if (strcmp(strGetStr(activeToken->value),"of") != 0){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            getNextToken();

            if (activeToken->tokenType != T_COLON){
                if (activeToken->tokenType == T_COMMA){
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            parseBuidInFunctionsStatus = argument();

            if(state->expParserReturnType != DATA_TYPE_STRING){raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);}

            char *tempVarName = generatorPopFirstStringFromList(gen->parserStack);
            symtablePushCode(symTable,"DEFVAR TF@!1");
            symtablePushCode(symTable,concatString(2,"MOVE TF@!1 ",tempVarName));
            
            // verification of: substring(<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                if (activeToken->tokenType == T_RIGHT_BRACKET){
                    raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            if (activeToken->tokenType != T_IDENTIFIER){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            if (strcmp(strGetStr(activeToken->value),"startingAt") != 0){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            getNextToken();

            if (activeToken->tokenType != T_COLON){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            getNextToken();

            parseBuidInFunctionsStatus = parseBuidInFunctionsStatus && argument();

            if(state->expParserReturnType != DATA_TYPE_INTEGER){raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);}

            tempVarName = generatorPopFirstStringFromList(gen->parserStack);
            symtablePushCode(symTable,"DEFVAR TF@!2");
            symtablePushCode(symTable,concatString(2,"MOVE TF@!2 ",tempVarName));

            // verification of: substring(<argument>,<argument>,<argument>
            if (activeToken->tokenType != T_COMMA){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            getNextToken();

            if (activeToken->tokenType != T_IDENTIFIER){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            if (strcmp(strGetStr(activeToken->value),"endingBefore") != 0){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            getNextToken();

            if (activeToken->tokenType != T_COLON){
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }
            getNextToken();

            parseBuidInFunctionsStatus = parseBuidInFunctionsStatus && argument();

            if(state->expParserReturnType != DATA_TYPE_INTEGER){raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);}

            tempVarName = generatorPopFirstStringFromList(gen->parserStack);
            symtablePushCode(symTable,"DEFVAR TF@!3");
            symtablePushCode(symTable,concatString(2,"MOVE TF@!3 ",tempVarName));
            
            // verification of: substring(<argument>,<argument>,<argument>)
            if (activeToken->tokenType != T_RIGHT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }

            symtablePushCode(symTable,"CALL $$substring");
            char *tempGeneratedName = generatorGenerateTempVarName(gen);
            char *tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(3,"MOVE ",tempNameWithPrefix," TF@%retval"));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_STRING;
            state->expParserReturnTypeNullable = true;
            state->changeToDouble = false;

            getNextToken();
            symtableFunctionCallEnd(symTable);
            break;
        case KW_ORD:
            // 67) <parseBuidInFunctions> -> ord(s : String)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"ord");
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_INTEGER,false);

            symtablePushCode(symTable,"");
            symtablePushCode(symTable,"#Start of build in function ord()");
            parseBuidInFunctionsStatus = argument();

            if(state->expParserReturnType != T_STRING){
                DEBUG_PRINTF("[Parser] Error function parameter should be string\n");
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
            }

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_COMMA){
                    getNextToken();
                    if(activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                        DEBUG_PRINTF("[Parser] Error function should have only one parameter\n");
                        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                    }
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }

            //Generator
            char *stringLengthVar = generatorGenerateTempVarName(gen);
            char *stringLengthVarPrefix = concatString(2,symtableGetVariablePrefix(symTable,stringLengthVar),stringLengthVar);

            symtablePushCode(symTable,concatString(2,"DEFVAR ",stringLengthVarPrefix));

            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));
            symtablePushCode(symTable,concatString(3,"MOVE ",tempNameWithPrefix," int@0"));

            char *argumentString = generatorPopFirstStringFromList(gen->parserStack);

            symtablePushCode(symTable,concatString(4, "STRLEN ", stringLengthVarPrefix, " ", argumentString));
            char *originalLabel  = generatorGenerateTempVarName(gen);
            symtablePushCode(symTable,concatString(5, "JUMPIFEQ ",originalLabel," ", stringLengthVarPrefix, " int@0"));
            symtablePushCode(symTable,concatString(5, "STRI2INT ",tempNameWithPrefix," ",argumentString, " int@0"));
            symtablePushCode(symTable,concatString(2,"LABEL ",originalLabel));
            symtablePushCode(symTable,"#End of build in function ord()");

            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_INT;
            state->expParserReturnTypeNullable = false;
            state->changeToDouble = false;

            getNextToken();

            symtableFunctionCallEnd(symTable);
            break;
        case KW_CHR:
            // 67) <parseBuidInFunctions> -> chr(i : Int)
            getNextToken();
            if (activeToken->tokenType != T_LEFT_BRACKET){
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }
            getNextToken();

            //Symtable
            symtableFunctionCallStart(symTable,"chr");
            symtableFunctionCallSetExpectedReturnType(symTable,DATA_TYPE_STRING,false);

            parseBuidInFunctionsStatus = argument();

            if(state->expParserReturnType != T_INT){
                DEBUG_PRINTF("[Parser] Error function parameter should be int\n");
                raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);       
            }

            if (activeToken->tokenType != T_RIGHT_BRACKET){
                if (activeToken->tokenType == T_COMMA){
                    getNextToken();
                    if(activeToken->tokenType == T_IDENTIFIER || activeToken->tokenType == T_INT || activeToken->tokenType == T_DOUBLE || activeToken->tokenType == T_STRING){
                        DEBUG_PRINTF("[Parser] Error function should have only one parameter\n");
                        raiseError(ERR_WRONG_NUMBER_OF_ARGUMENTS);
                    }
                }
                DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
                return false;
            }

            //Generator
            tempGeneratedName = generatorGenerateTempVarName(gen);
            tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
            symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

            symtablePushCode(symTable,concatString(4, "INT2CHAR ", tempNameWithPrefix, " ", generatorPopFirstStringFromList(gen->parserStack)));
            generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);

            state->expParserReturnType = T_STRING;
            state->expParserReturnTypeNullable = false;
            state->changeToDouble = false;

            getNextToken();

            symtableFunctionCallEnd(symTable);
            break;
        default:
            DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",false);
            return false;
    }
    DEBUG_PRINTF("[Parser] Leaving function parseBuidInFunctions() with %d ...\n",parseBuidInFunctionsStatus);
    return parseBuidInFunctionsStatus;
}

void parseFunctionCall(){
    bool parseFunctionCallStatus = false;
    getNextToken();
    DEBUG_PRINTF("[Parser] Token: %s\n",getTokenName(activeToken->tokenType));
    DEBUG_PRINTF("[Parser] Entering function parseFunctionCall()...\n");

    if(activeToken->tokenType == T_IDENTIFIER){
        char *functionName = concatString(1,strGetStr(activeToken->value));
        symtableSetFunctionCallName(symTable,functionName);
        state->expParserReturnType = symtableGetVariableType(symTable,functionName);
        state->expParserReturnTypeNullable = symtableGetVariableNullable(symTable,functionName);
        state->changeToDouble = false;

        getNextToken();
        if(activeToken->tokenType != T_LEFT_BRACKET){ 
            DEBUG_PRINTF("[Parser] Syntax error left bracket missing\n");
            raiseError(ERR_SYNTAX);
        }
        getNextToken();

        //Generator
        symtablePushCodeCreateFrame(symTable);

        //Symtable
        symtableFunctionCallStart(symTable,NULL);
        symtableFunctionCallSetExpectedReturnType(symTable,symtableGetVariableType(symTable,functionName),symtableGetVariableNullable(symTable,functionName));

        parseFunctionCallStatus = arguments();

        int i = symtableFunctionCallGetNumberOfParameters(symTable);
        char *result = allocateString("Toto zde musime nechat jinak to hodi segfault. Tuto poznamku muzete ingnorovat protoze se stejne prepise :)");
        while(i > 0){
            snprintf(result, sizeof(result), "%d", i);
            symtablePushCode(symTable,concatString(2,"DEFVAR TF@!",result));
            symtablePushCode(symTable,concatString(4,"MOVE TF@!",result," ",generatorPopFirstStringFromList(gen->parserStack)));
            i--;
        }

        state->expParserReturnType = symtableGetVariableType(symTable,functionName);
        state->expParserReturnTypeNullable = symtableGetVariableNullable(symTable,functionName);
        state->changeToDouble = false;

        symtablePushCode(symTable,symTable->lastFunctionCall);

        char *tempGeneratedName = generatorGenerateTempVarName(gen);
        char *tempNameWithPrefix = concatString(2,symtableGetVariablePrefix(symTable,tempGeneratedName),tempGeneratedName);
        symtablePushCode(symTable,concatString(2,"DEFVAR ",tempNameWithPrefix));

        symtablePushCode(symTable,concatString(3,"MOVE ",tempNameWithPrefix," TF@%retval"));
        generatorPushStringFirstToList(gen->parserStack,tempNameWithPrefix);
        symtableFunctionCallEnd(symTable);
    }else{
        parseFunctionCallStatus = parseBuidInFunctions();
    }
    token *tempToken = tokenInit();
    tempToken->tokenType = activeToken->tokenType;
    tempToken->lastChar = activeToken->lastChar;
    char *string = allocateString(strGetStr(activeToken->value));
    strSetString(tempToken->value,string);
    DEBUG_PRINTF("[Parser] Pushing token %s to tokenQueue\n",getTokenName(tempToken->tokenType));
    listPushBack(state->tokenQueue,tempToken);
    if (parseFunctionCallStatus){
        DEBUG_PRINTF("[Parser] Successfully leaving function parseFunctionCall()\n");
        return;
    }
    DEBUG_PRINTF("[Parser] Leaving function parseFunctionCall() with syntax error\n");
    raiseError(ERR_SYNTAX);
}