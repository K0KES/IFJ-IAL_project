#include "scanner.h"
#include "error.h"
#pragma once

void tokenClear (token* token) {
    strClear(token->value);
    token->tokenType = T_NO_TOKEN;
    token->lastChar = '\0';
}

int getToken(token *token, int charNumber, int lineNumber) {
    char c = '\0';
    enum state state = S_START;
    // DEBUG_PRINTF("\n");
    char lastChar = '\0';

    int multilineExit = 0;
    
    int stringHexaCount = 0;
    int stringHexaFirst = 0;
    int stringHexaSecond = 0;

    int spaceCounter = 0;
    int firstCharCount = 0;
    int foundFirstChar = 0;
    int lastCharCount = 0;
    int spaceCount2 = 0;

    int firstNewLine = 0;

    int fromNewLineState = 0;

    string* multiLineString;
    while (c != EOF) {
        c = getc(stdin);
        if (state != S_BLOCK_COMMENT && state != S_LINE_COMMENT && state != S_BLOCK_LINE_COMMENT && c != '/') { /*DEBUG_PRINTF("%c\n", c);*/ }
        // if (lineNumber > 10) {  raiseError(ERR_LEXICAL); }
        switch (state) {
            /////////////////////////  
            //first starting STATE
            /////////////////////////  
            case S_START:
                charNumber++;
                token->lastChar = lastChar;
                switch (c) {
                    case '\t':
                        charNumber++;
                        charNumber++;
                        break;
                    case ' ':
                        break;
                    case '\n':
                    case '\r':
                        lineNumber++;
                        charNumber = 0;
                        strAddChar(token->value, c);
                        //DEBUG_PRINTF("\nEOL\n");
                        state = S_NEW_LINE;
                        break;
                    /////////////////////////  
                    //STATES chars
                    /////////////////////////  
                    case '+':
                        strAddChar(token->value, c);
                        state = S_INCREMENT;
                        break;
                    case '-':
                        strAddChar(token->value, c);
                        state = S_DECREMENT;
                        break;
                    case '<':
                        strAddChar(token->value, c);
                        state = S_LESS_EQUAL;
                        break;
                    case '>':
                        strAddChar(token->value, c);
                        state = S_GREATER_EQUAL;
                        break;
                    case '=':
                        strAddChar(token->value, c);
                        state = S_EQUAL;
                        break;
                    case '*':
                        strAddChar(token->value, c);
                        state = S_VAR_MUL_VAR;
                        break;
                    case '?':
                        strAddChar(token->value, c);
                        state = S_NIL_OP;
                        break;
                    case '!':
                        strAddChar(token->value, c);
                        state = S_NOT_EQUAL;
                        break;
                    case '_':
                        strAddChar(token->value, c);
                        state = S_UNDERSCORE_IDENTIFIER;
                        break;
                    case '/':
                        state = S_BLOCK_LINE_COMMENT;
                        break;
                    case '"':
                        strAddChar(token->value, c);
                        state = S_MULTILINE_LINE_STRING;
                        break;
                    /////////////////////////    
                    //SIMPLE NO STATE chars
                    /////////////////////////  
                    case '(':
                        token->tokenType = T_LEFT_BRACKET;
                        strAddChar(token->value, c);
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                        break;
                    case ')':
                        token->tokenType = T_RIGHT_BRACKET;
                        strAddChar(token->value, c);
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                        break;
                    case '{':
                        token->tokenType = T_LEFT_CURLY_BRACKET;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return LEX_OK;
                        break;
                    case '}':
                        token->tokenType = T_RIGHT_CURLY_BRACKET;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return LEX_OK;
                        break;
                    case ':':
                        token->tokenType = T_COLON;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return LEX_OK;
                        break;
                    case ';':
                        token->tokenType = T_SEMICOLON;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return LEX_OK;
                        break;
                    case ',':
                        token->tokenType = T_COMMA;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return LEX_OK;
                        break;
                    case EOF:
                        token->tokenType = T_EOF;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        //raiseError(ERR_LEXICAL);
                       
                        // return LEX_OK;
                        return LEX_ERROR;
                        break;
                    /////////////////////////  
                    //ID || KW || INT || DOUBLE
                    /////////////////////////  
                    default:
                        if (isalpha(c)) { 
                            strAddChar(token->value, c);  
                            state = S_IDENTIFIER_KW;
                        }

                        else if(isalnum(c)) {
                            strAddChar(token->value, c);  
                            state = S_INT;
                        }
                        else if (!isalnum(c)) { raiseError(ERR_LEXICAL); }
                        break;
                }
                if (state != S_NEW_LINE) lastChar = c;
                break;
            /////////////////////////  
            //STATES continues
            /////////////////////////  
            case S_NEW_LINE:
                if (lastChar == '/' && (c != '/' && c != '*')) { 
                    token->tokenType = T_EOL;
                    ungetc(c, stdin);
                    ungetc(lastChar, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                 }
                switch (c) {
                case '\n':
                case '\r':
                    lineNumber++;
                    charNumber = 0;
                    //DEBUG_PRINTF("\nEOL\n"); 
                    lastChar = '\0';
                    break;
                case ' ':
                    charNumber++;
                    lastChar = ' ';
                    break;
                case '\t':
                    charNumber++;
                    charNumber++;
                    charNumber++;
                    lastChar = '\t';
                    break;
                case '/':
                    if (lastChar == '/') { //we get two forward slashes, 
                        ungetc (c, stdin); //we unget them and transition to line comment 
                        ungetc('/', stdin); //so we can return only one newline token
                        state = S_START; //for many newline chars
                        strClear(token->value);
                        fromNewLineState = 1;
                        }
                    if (state != S_START) { lastChar = '/'; }
                    break;
                case '*':
                    if (lastChar == '/') { //transition to block comment
                        ungetc (c, stdin); 
                        ungetc('/', stdin);
                        state = S_START; 
                        strClear(token->value);
                        fromNewLineState = 1;
                        }
                    else {
                        token->tokenType = T_EOL;
                        ungetc(c, stdin);
                        ungetc(lastChar, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                default:
                    token->tokenType = T_EOL;
                    ungetc(c, stdin);
                    if (lastChar == ' ' || lastChar == '\t') { ungetc(lastChar, stdin); }
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN ++ OR +
            ///////////////////////// 
            case S_INCREMENT:
                if (c == '=') { 
                    token->tokenType = T_INCREMENT;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_PLUS;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN *= OR *
            /////////////////////////
            case S_VAR_MUL_VAR:
                if (c == '=') { 
                    token->tokenType = T_VAR_MUL_VAR;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_MULTIPLICATION;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN -- OR - OR ->
            /////////////////////////
            case S_DECREMENT:
                if (c == '=') { 
                    token->tokenType = T_DECREMENT;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else if (c == '>') {
                    token->tokenType = T_ARROW;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_MINUS;
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    ungetc(c, stdin);
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN < OR <=
            /////////////////////////
            case S_LESS_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_LESS_EQUAL;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_LESS;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN > OR >=
            /////////////////////////
            case S_GREATER_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_GREATER_EQUAL;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_GREATER;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN == OR =
            /////////////////////////
            case S_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_EQUAL; 
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_ASSIGNMENT;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN ? OR ??
            /////////////////////////
            case S_NIL_OP:
                if (c == '?') { 
                    token->tokenType = T_NIL_OP; 
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_NULLABLE;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN != OR !
            /////////////////////////
            case S_NOT_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_NOT_EQUAL; 
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                else {
                    token->tokenType = T_NOT;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                }
                break;
            /////////////////////////    
            //TOKEN _ OR STATE ID/KW
            /////////////////////////
            case S_UNDERSCORE_IDENTIFIER:
                switch (c) {
                    //it is _
                    case '\t':
                    case ' ':
                    case '\r':
                    case '\n':
                        token->tokenType = KW_UNDERSCORE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                        break;
                    //it is identifier
                    default:
                        if (isalnum(c) || c == '_') {
                            state = S_IDENTIFIER_KW;
                            strAddChar(token->value, c);
                            charNumber++;
                        }
                        break;
                }
                break;
            /////////////////////////    
            //TOKEN ID OR KW
            /////////////////////////
            case S_IDENTIFIER_KW:
                switch (c) {
                //KEYWORD
                case '\r':
                case '\t':
                case ' ':
                case '\n':
                case '(':
                case ')':
                case ',':
                case '{':
                case '}':
                case '?':
                case EOF:
                    //DEBUG_PRINTF("\ncurrent token val='");
                    // strPrint(token->value);
                    // DEBUG_PRINTF("\n");
                    if (strCmpConstStr(token->value, "Double") == 0) {
                        token->tokenType = KW_DOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "else") == 0) {
                        token->tokenType = KW_ELSE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "func") == 0) {
                        token->tokenType = KW_FUNC;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "if") == 0) {
                        token->tokenType = KW_IF;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "Int") == 0) {
                        token->tokenType = KW_INT;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "let") == 0) {
                        token->tokenType = KW_LET;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "nil") == 0) {
                        token->tokenType = KW_NIL;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "return") == 0) {
                        token->tokenType = KW_RETURN;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "String") == 0) {
                        token->tokenType = KW_STRING;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "var") == 0) {
                        token->tokenType = KW_VAR;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "while") == 0) {
                        token->tokenType = KW_WHILE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "readString") == 0) {
                        token->tokenType = KW_READSTRING;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "readInt") == 0) {
                        token->tokenType = KW_READINT;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "readDouble") == 0) {
                        token->tokenType = KW_READDOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "write") == 0) {
                        token->tokenType = KW_WRITE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "Int2Double") == 0) {
                        token->tokenType = KW_INT_TO_DOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "Double2Int") == 0) {
                        token->tokenType = KW_DOUBLE_TO_INT;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "length") == 0) {
                        token->tokenType = KW_LENGTH;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "substring") == 0) {
                        token->tokenType = KW_SUBSTRING;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "ord") == 0) {
                        token->tokenType = KW_ORD;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "chr") == 0) {
                        token->tokenType = KW_CHR;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else {
                        token->tokenType = T_IDENTIFIER;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }       
                    break;
                //IDENTIFIER
                default:
                    if (isalnum(c) || c == '_') {
                        strAddChar(token->value, c);
                        charNumber++;
                    }
                    else {
                        token->tokenType = T_IDENTIFIER;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN INT OR STATE DOUBLE/EXPONENT INT
            /////////////////////////
            case S_INT:
                switch (c) {
                case '.':
                    state = S_DOUBLE;
                    strAddChar(token->value, c);
                    charNumber++;
                    lastChar = c;
                    break;
                case 'e':
                case 'E':
                    strAddChar(token->value, c);
                    charNumber++;
                    lastChar = c;
                    state = S_EXPONENT_INT;
                    break;
                default:
                    if (!isalpha(c) && isalnum(c)) {  
                        strAddChar(token->value, c);
                        charNumber++;
                    }
                    else {
                        token->tokenType = T_INT;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN INT with EXPONENT (2e35)
            /////////////////////////
            case S_EXPONENT_INT:
                switch (c) {
                case 'e':
                case 'E':
                case '.':
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                     raiseError(ERR_LEXICAL);
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                case EOF:
                    if (lastChar == 'e' || lastChar == 'E' || lastChar == '+' || lastChar == '-') {
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                         raiseError(ERR_LEXICAL);    
                    }
                    token->tokenType = T_INT;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                case '+':
                case '-':
                    if (lastChar == 'e' || lastChar == 'E') {
                        strAddChar(token->value, c);
                        charNumber++;
                        lastChar = c;
                    }
                    else if (lastChar == '+' || lastChar == '-') {
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                         raiseError(ERR_LEXICAL);
                    }
                    else {
                        token->tokenType = T_INT;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                default:
                    if (!isalpha(c) && isalnum(c)) {  
                        strAddChar(token->value, c);
                        charNumber++;
                        lastChar = c;
                    }
                    else if (lastChar == '+' || lastChar == '-') {
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                         raiseError(ERR_LEXICAL);
                    }
                    else {
                        token->tokenType = T_INT;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN DOUBLE (10.17) OR STATE DOUBLE with EXPONENT
            /////////////////////////
            case S_DOUBLE:
                    if (lastChar == '.' && (c < 48 || c > 57)) { raiseError(ERR_LEXICAL); }
                    if (!isalpha(c) && isalnum(c)) {  
                        strAddChar(token->value, c);
                        charNumber++;
                        lastChar = c;
                    }
                    else if (c == 'e' || c == 'E') {
                        strAddChar(token->value, c);
                        charNumber++;
                        lastChar = c;
                        state = S_EXPONENT_DOUBLE;
                    }
                    else {
                        token->tokenType = T_DOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                break;
            /////////////////////////    
            //TOKEN DOUBLE OR DOUBLE with EXPONENT (10.45e+78)
            /////////////////////////
            case S_EXPONENT_DOUBLE:
                switch (c) {
                case 'e':
                case 'E':
                case '.':
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                     raiseError(ERR_LEXICAL);
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                case EOF:
                    if (lastChar == 'e' || lastChar == 'E' || lastChar == '+' || lastChar == '-') {
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                         raiseError(ERR_LEXICAL);    
                    }
                    token->tokenType = T_DOUBLE;
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                case '+':
                case '-':
                    if (lastChar == 'e' || lastChar == 'E') {
                        strAddChar(token->value, c);
                        charNumber++;
                        lastChar = c;
                    }
                    else if (lastChar == '+' || lastChar == '-') {
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                         raiseError(ERR_LEXICAL);
                    }
                    else {
                        token->tokenType = T_DOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                default:
                    if (!isalpha(c) && isalnum(c)) {  
                        strAddChar(token->value, c);
                        charNumber++;
                        lastChar = c;
                    }
                    else if (lastChar == '+' || lastChar == '-') {
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                         raiseError(ERR_LEXICAL);
                    }
                    else {
                        token->tokenType = T_DOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN / OR /= OR STATE LINE COMMENT OR BLOCK COMMENT
            /////////////////////////
            case S_BLOCK_LINE_COMMENT:
                switch (c) {
                case '/':
                    state = S_LINE_COMMENT;
                    break;
                case '*':
                    state = S_BLOCK_COMMENT;
                    multilineExit++;
                    break;
                case '=':
                    token->tokenType = T_VAR_DIV_VAR;
                    charNumber++;
                    strAddChar(token->value, lastChar);
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                default:
                    strAddChar(token->value, lastChar);
                    token->tokenType = T_DIVISION;
                    //DEBUG_PRINTF("%c", lastChar);
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                }
                break;
            /////////////////////////    
            //LINE COMMENT
            /////////////////////////
            case S_LINE_COMMENT:
                if (c == '\n' || c == EOF) {ungetc(c, stdin); state = S_START; lastChar = '\0'; }
                break;
            /////////////////////////    
            //BLOCK COMMENT
            /////////////////////////
            case S_BLOCK_COMMENT:
                if (c == '\n' || c == '\r') { lineNumber++; }
                if (c == '/' && lastChar == '*') {
                    multilineExit--;
                    if (multilineExit == 0) { if (fromNewLineState == 1) { ungetc('\n', stdin); lineNumber--;} state = S_START; }
                }
                if (c == '*' && lastChar == '/') { multilineExit++; }
                if (c == EOF && multilineExit > 0) {  /*DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);*/  raiseError(ERR_LEXICAL); }
                lastChar = c;
                if (multilineExit == 0) lastChar = '\0';
                break;
            /////////////////////////    
            //TOKEN non_empty STRING OR STATE MULTILINE STRING """ """
            /////////////////////////
            case S_MULTILINE_LINE_STRING:
                switch (c) {
                case '"':
                    charNumber++;
                    strAddChar(token->value, c);
                    state = S_MULTILINE_LINE_STRING_CHECK;
                    break;
                case EOF:
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                     raiseError(ERR_LEXICAL);
                    break;
                default:
                    ungetc(c, stdin);
                    state = S_STRING;
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN STRING ""
            /////////////////////////
            case S_STRING:
                switch (c) {
                case '\t':
                    charNumber++;
                    charNumber++;
                    charNumber++;
                    strAddChar(token->value, c);
                    break;
                //its escapable char
                case '\\':
                    state = S_STRING_ESCAPE;
                    lastChar = c;
                    break;
                case '\"':
                    strAddChar(token->value, c);
                    charNumber++;
                    state = S_START;
                    token->tokenType = T_STRING;
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                case '\r':
                case '\n':
                case EOF:
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                     raiseError(ERR_LEXICAL);
                default:
                    charNumber++;
                    lastChar = c;
                    strAddChar(token->value, c);              
                    break;
                }
                break;
            /////////////////////////    
            //ESCAPABLE CHAR
            /////////////////////////
            case S_STRING_ESCAPE:
                switch (c) {
                case 'n':
                case '\"':
                case 'r':
                case 't':
                case '\\':
                    strAddChar(token->value, lastChar);
                    strAddChar(token->value, c);
                    charNumber++;
                    charNumber++;
                    state = S_STRING;
                    break;
                //SPECIAL STATE FOR \u{dd}
                case 'u':
                    state = S_STRING_HEXA;
                    break;
                //ERROR
                default:
                    charNumber++;
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    raiseError(ERR_LEXICAL);
                    break;
                }
                break;
            /////////////////////////    
            // \u{dd}
            /////////////////////////
            case S_STRING_HEXA:
                // printf("%c - > ",c);
                switch (c) {
                case '{':
                    if (lastChar == '{') { raiseError(ERR_LEXICAL); }
                    lastChar = c;
                    break;
                case '}':
                    if (stringHexaCount > 2 ) { raiseError(ERR_LEXICAL); }
                    if (lastChar != '{') { raiseError(ERR_LEXICAL); }
                    stringHexaFirst = (stringHexaFirst << 4) + stringHexaSecond;
                    // printf("result:%d\n", stringHexaFirst);
                    strAddChar(token->value, stringHexaFirst);
                    
                    state = S_STRING;
                    break;
                case 'a':
                case 'A':
                    if (stringHexaCount == 0) {
                        stringHexaFirst = 10;
                    }
                    else if (stringHexaCount == 1) { stringHexaSecond = 10; }
                    stringHexaCount++;
                    break;
                case 'b':
                case 'B':
                    if (stringHexaCount == 0) {
                        stringHexaFirst = 11;
                    }
                    else if (stringHexaCount == 1) { stringHexaSecond = 11; }
                    stringHexaCount++;
                    break;
                case 'c':
                case 'C':
                    if (stringHexaCount == 0) {
                        stringHexaFirst = 12;
                    }
                    else if (stringHexaCount == 1) { stringHexaSecond = 12; }
                    stringHexaCount++;
                    break;
                case 'd':
                case 'D':
                    if (stringHexaCount == 0) {
                        stringHexaFirst = 13;
                    }
                    else if (stringHexaCount == 1) { stringHexaSecond = 13; }
                    stringHexaCount++;
                    break;
                case 'e':
                case 'E':
                    if (stringHexaCount == 0) {
                        stringHexaFirst = 14;
                    }
                    else if (stringHexaCount == 1) { stringHexaSecond = 14; }
                    stringHexaCount++;
                    break;
                case 'f':
                case 'F':
                    if (stringHexaCount == 0) {
                        stringHexaFirst = 15;
                    }
                    else if (stringHexaCount == 1) { stringHexaSecond = 15; }
                    stringHexaCount++;
                    break;
                
                default:
                    if (!isalpha(c) && isalnum(c)) {
                        if (stringHexaCount == 0) {
                            stringHexaFirst = c - 48;
                            }
                        else if (stringHexaCount == 1) { stringHexaSecond = c - 48; }
                        stringHexaCount++;
                        }
                    else { raiseError(ERR_LEXICAL); }
                    break;
                }
                // printf("%d", stringHexaFirst);
                // printf(" %d", stringHexaSecond);
                // printf(" count:%d\n", stringHexaCount);
                break;
            /////////////////////////    
            //TOKEN empty STRING or STATE MULTILINE STRING
            /////////////////////////
            case S_MULTILINE_LINE_STRING_CHECK:
                switch (c) {
                case '"':
                    charNumber++;
                    strAddChar(token->value, c);
                    state = S_MULTILINE_STRING;
                    //new string, where we copy full multiline string without entering """ sequence
                    multiLineString = (string*)malloc(sizeof(string));
                    if (multiLineString == NULL) { raiseError(ERR_INTERNAL); }
                    lastChar = c;
                    break;
                
                default:
                    ungetc(c, stdin);
                    state = S_START;
                    token->tokenType = T_STRING;
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                }
                break;
            /////////////////////////    
            //TOKEN MULTILINE STRING
            /////////////////////////
            case S_MULTILINE_STRING:
                switch (c) {
                case '\t':
                    charNumber++;
                    charNumber++;
                    charNumber++;
                    strAddChar(multiLineString, c);
                    spaceCounter+=3;
                    break;
                case '\r':
                case '\n':
                    // if (firstNewLine == 0) firstNewLine++;

                    // if (firstNewLine > 0) { foundFirstChar = 1; spaceCounter};
                    lineNumber++;
                    charNumber = 0;
                    strAddChar(multiLineString, c);

                    if (foundFirstChar == 0) spaceCounter = 0;
                    break;
                case '\\':
                    firstCharCount = 1;
                    state = S_MULTILINE_STRING_ESCAPE;
                    lastChar = c;
                    break;
                case ' ':
                    if (foundFirstChar == 0) spaceCounter++;
                    spaceCount2++;
                    strAddChar(multiLineString, c);
                    charNumber++;
                    break;
                case EOF:
                    raiseError(LEX_ERROR);
                    break;
                case '"':
                    // if (lastChar != '\n') {
                    //     // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    //     raiseError(ERR_LEXICAL);
                    // }
                    strAddChar(multiLineString, c);
                    charNumber++;
                    state = S_MULTILINE_STRING_EXIT;
                    lastChar = '\0';
                    break;
                //ERROR
                default:
                    foundFirstChar = 1;
                    lastCharCount++;
                    if (lastChar == '"') {
                        charNumber++;
                        // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        raiseError(ERR_LEXICAL);
                    }
                    strAddChar(multiLineString, c);
                    charNumber++;
                    break;
                }
                if (foundFirstChar == 0)  firstCharCount++;
                // if (foundFirstChar == 0) printf("\nfirstCount = %d, spaceCount = %d\n", firstCharCount, spaceCounter);
                if (state != S_MULTILINE_STRING_EXIT) { lastChar = c; } 
                break;
            /////////////////////////    
            //STATE for handeling exit of multiline string
            /////////////////////////
            case S_MULTILINE_STRING_EXIT:
                switch (c) {
                case '"':
                    if (lastChar == '"') {
                        strAddChar(multiLineString, c);
                        // charNumber++;
                        // state = S_START;
                        // token->tokenType = T_MULTILINE_STRING;
                        // token->position->charNumber = charNumber;
                        // token->position->lineNumber = lineNumber;

                        //call function to handle indent and check for errors
                        string* checkedMultiLineString = multilineStringCheck(multiLineString, firstCharCount-spaceCounter-1);

                        charNumber++;
                        state = S_START;
                        token->tokenType = T_MULTILINE_STRING;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strFree(token->value);
                        token->value = checkedMultiLineString;


                        return LEX_OK;
                    }
                    strAddChar(multiLineString, c);
                    charNumber++;
                    lastChar = c;
                    break;
                //we have first quote and we get something else than quote => error
                default:
                    charNumber++;
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    raiseError(ERR_LEXICAL);
                    break;
                }
                break;
            /////////////////////////    
            //STATE for ESCAPE CHARS
            /////////////////////////
            case S_MULTILINE_STRING_ESCAPE:
                switch (c) {
                case 'n':
                case '\"':
                case 'r':
                case 't':
                case '\\':
                    strAddChar(token->value, lastChar);
                    strAddChar(token->value, c);
                    charNumber++;
                    charNumber++;
                    state = S_MULTILINE_STRING;
                    break;
                //ERROR
                default:
                    charNumber++;
                    // DEBUG_PRINTF("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    raiseError(ERR_LEXICAL);
                    break;
                }
                break;
        }
    }
    token->tokenType = T_EOF;
    token->lastChar = lastChar;
    token->position->charNumber = charNumber;
    token->position->lineNumber = lineNumber;
    strAddChar(token->value, c);
    // return LEX_OK;
    return LEX_ERROR;
}

string* multilineStringCheck (string* multiLine, int firstCharPos) {

    int indent = 0;
    if (multiLine->str[0] != '\n') { raiseError(ERR_LEXICAL); }

    char beforeQuote = multiLine->str[multiLine->length-4]; //first char before ending """ quotes
    int i = 1;


    string* newString = (string*)malloc(sizeof(string));
    if (newString == NULL) { raiseError(ERR_INTERNAL); }

    strAddChar(newString, '"');
    strAddChar(newString, '"');
    strAddChar(newString, '"');

    //until we meet first newline, we count the indent before exit sequence of """
    while (beforeQuote != '\n') {
        if (beforeQuote == ' ') { indent++; } //counting indent
        else if (beforeQuote != ' ' && beforeQuote != '\n') { raiseError(ERR_LEXICAL); } //there is something else before """ than space or newline => error
    
        beforeQuote = multiLine->str[multiLine->length-4-i];
        i++;
    }

    // printf("\nEnd of string pos: %d\n", endOfStringPos);
    // printf("\n\n INDENT = %d \n\n", indent);
    
    int k = 0;
    int j = -1;
    char newLineStart;
    int currentIndent = 0;
    for (int k = 0; k < multiLine->length; k++) {
        newLineStart = multiLine->str[k];
        // printf("\n znak: '%c'\n", newLineStart);
            if (newLineStart == '\n') {
                strAddChar(newString, newLineStart);
                k++;
                newLineStart = multiLine->str[k];
                // printf("\n znak: '%c'\n", newLineStart);
                // printf("\nENTERING NEWLINE\n");
                
                while (newLineStart == ' ') {
                    // printf("\n znak: '%c'\n", newLineStart);
                    if (currentIndent == indent) { break; }
                    currentIndent++;
                    k++;
                    newLineStart = multiLine->str[k];
                    j = k;
                }
                //indents of lines that starts with something else than \n are not equal => error
                if (currentIndent != indent && (j == k)) { raiseError(ERR_LEXICAL); }
                currentIndent = 0;
            }
        strAddChar(newString, newLineStart);
    }

    return newString;

}


const char* getTokenName(enum tokenType tokenType) {
   switch (tokenType) {
        case T_NO_TOKEN: return "T_NO_TOKEN";
        case T_WHITESPACE: return "T_WHITESPACE";
        case T_EOF: return "T_EOF";
        case T_EOL: return "T_EOL";
        case T_IDENTIFIER: return "T_IDENTIFIER";
        case T_LEFT_BRACKET: return "T_LEFT_BRACKET";
        case T_RIGHT_BRACKET: return "T_RIGHT_BRACKET";
        case T_LEFT_CURLY_BRACKET: return "T_LEFT_CURLY_BRACKET";
        case T_RIGHT_CURLY_BRACKET: return "T_RIGHT_CURLY_BRACKET";
        case T_PLUS: return "T_PLUS";
        case T_MINUS: return "T_MINUS";
        case T_MULTIPLICATION: return "T_MULTIPLICATION";
        case T_DIVISION: return "T_DIVISION";
        case T_LESS: return "T_LESS";
        case T_LESS_EQUAL: return "T_LESS_EQUAL";
        case T_GREATER: return "T_GREATER";
        case T_GREATER_EQUAL: return "T_GREATER_EQUAL";
        case T_EQUAL: return "T_EQUAL";
        case T_NOT_EQUAL: return "T_NOT_EQUAL";
        case T_NIL_OP: return "T_NIL_OP";
        case T_ASSIGNMENT: return "T_ASSIGNMENT";
        case T_INCREMENT: return "T_INCREMENT";  
        case T_DECREMENT: return "T_DECREMENT";  
        case T_NOT: return "T_NOT";
        case T_INT: return "T_INT";
        case T_DOUBLE: return "T_DOUBLE";
        case T_STRING: return "T_STRING";
        case T_MULTILINE_STRING: return "T_MULTILINE_STRING";
        case T_NULLABLE: return "T_NULLABLE";
        case T_SEMICOLON: return "T_SEMICOLON";
        case T_COLON: return "T_COLON";
        case T_COMMA: return "T_COMMA";
        case T_ARROW: return "T_ARROW";
        case T_END: return "T_END";
        case T_E: return "T_E";
        case KW_DOUBLE: return "KW_DOUBLE";
        case KW_FUNC: return "KW_FUNC";
        case KW_RETURN: return "KW_RETURN";
        case KW_INT: return "KW_INT";
        case KW_STRING: return "KW_STRING";
        case KW_NIL: return "KW_NIL";
        case KW_ELSE: return "KW_ELSE";
        case KW_IF: return "KW_IF";
        case KW_WHILE: return "KW_WHILE";
        case KW_VAR: return "KW_VAR";
        case KW_LET: return "KW_LET";
        case KW_UNDERSCORE: return "KW_UNDERSCORE";
        case KW_READSTRING: return "KW_READSTRING";
        case KW_READINT: return "KW_READINT";
        case KW_READDOUBLE: return "KW_READDOUBLE";
        case KW_WRITE: return "KW_WRITE";
        case KW_INT_TO_DOUBLE: return "KW_INT_TO_DOUBLE";
        case KW_DOUBLE_TO_INT: return "KW_DOUBLE_TO_INT";
        case KW_LENGTH: return "KW_LENGTH";
        case KW_SUBSTRING: return "KW_SUBSTRING";
        case KW_ORD: return "KW_ORD";
        case KW_CHR: return "KW_CHR";
   }
   return "";
}
