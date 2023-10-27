#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "scanner.h"


void tokenClear (token* token) {
    strClear(token->value);
    token->tokenType = T_NO_TOKEN;
    token->lastChar = '\0';
}

int getToken(token *token, int charNumber, int lineNumber) {
    char c;
    enum state state = S_START;
    //printf("'");
    char lastChar = '\0';
    while (c != EOF) {
        c = getc(stdin);
        if (state != S_BLOCK_COMMENT && state != S_LINE_COMMENT && state != S_BLOCK_LINE_COMMENT && c != '/') { printf("%c\n", c); }
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
                        printf("\nEOL\n");
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
                    //EASY chars
                    /////////////////////////  
                    case '*':
                        token->tokenType = T_MULTIPLICATION;
                        strAddChar(token->value, c);
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                        break;
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
                        token->tokenType = T_COLON;
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
                        // return LEX_ERROR;
                        return LEX_OK;
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
                        break;
                }
                lastChar = c;
                break;
            /////////////////////////  
            //STATES continues
            /////////////////////////  
            case S_NEW_LINE:
                switch (c) {
                case '\n':
                case '\r':
                    lineNumber++;
                    charNumber = 0;
                    printf("\nEOL\n"); 
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
            case S_IDENTIFIER_KW:
                switch (c) {
                //KEYWORD
                case '\r':
                case '\t':
                case ' ':
                case '\n':
                case '(':
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
                    else if (strCmpConstStr(token->value, "Int1Double") == 0) {
                        token->tokenType = KW_INT_TO_DOUBLE;
                        ungetc(c, stdin);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    else if (strCmpConstStr(token->value, "Double1Int") == 0) {
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
            case S_INT:
                switch (c) {
                case '.':
                    state = S_DOUBLE;
                    strAddChar(token->value, c);
                    charNumber++;
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
            case S_EXPONENT_INT:
                switch (c) {
                case 'e':
                case 'E':
                case '.':
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if (lastChar == 'e' || lastChar == 'E' || lastChar == '+' || lastChar == '-') {
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;    
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
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;
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
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;
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
            case S_DOUBLE:
                    if (!isalpha(c) && isalnum(c)) {  
                        strAddChar(token->value, c);
                        charNumber++;
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
            case S_EXPONENT_DOUBLE:
                switch (c) {
                case 'e':
                case 'E':
                case '.':
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if (lastChar == 'e' || lastChar == 'E' || lastChar == '+' || lastChar == '-') {
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;    
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
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;
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
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;
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
            case S_BLOCK_LINE_COMMENT:
                switch (c) {
                case '/':
                    state = S_LINE_COMMENT;
                    break;
                case '*':
                    state = S_BLOCK_COMMENT;
                    break;
                default:
                    strAddChar(token->value, lastChar);
                    token->tokenType = T_DIVISION;
                    //printf("%c", lastChar);
                    ungetc(c, stdin);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return LEX_OK;
                    break;
                }
                break;
            case S_LINE_COMMENT:
                if (c == '\n') { lineNumber++; lastChar = c; charNumber = 0; state = S_START; }
                break;
            case S_BLOCK_COMMENT:
                if (c == '\n' || c == '\r') { lineNumber++; }
                if (c == '/' && lastChar == '*') { state = S_START; }
                lastChar = c;
                break;
            case S_MULTILINE_LINE_STRING:
                switch (c) {
                case '"':
                    charNumber++;
                    strAddChar(token->value, c);
                    state = S_MULTILINE_LINE_STRING_CHECK;
                    break;
                case EOF:
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                    break;
                default:
                    ungetc(c, stdin);
                    state = S_STRING;
                    break;
                }
                break;
            case S_STRING:
                switch (c) {
                case '\t':
                    charNumber++;
                    charNumber++;
                    charNumber++;
                    strAddChar(token->value, c);
                    break;
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
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                default:
                    charNumber++;
                    lastChar = c;
                    strAddChar(token->value, c);              
                    break;
                }
                break;
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
                //ERROR
                default:
                    charNumber++;
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                    break;
                }
                break;
            case S_MULTILINE_LINE_STRING_CHECK:
                switch (c) {
                case '"':
                    charNumber++;
                    strAddChar(token->value, c);
                    state = S_MULTILINE_STRING;
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
            case S_MULTILINE_STRING:
                switch (c) {
                case '\t':
                    charNumber++;
                    charNumber++;
                    charNumber++;
                    strAddChar(token->value, c);
                    break;
                case '\r':
                case '\n':
                    lineNumber++;
                    charNumber = 0;
                    strAddChar(token->value, c);
                    break;
                case '\\':
                    state = S_MULTILINE_STRING_ESCAPE;
                    lastChar = c;
                    break;
                case '"':
                    if (lastChar != '\n') {
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;
                    }
                    strAddChar(token->value, c);
                    charNumber++;
                    state = S_MULTILINE_STRING_EXIT;
                    lastChar = '\0';
                    break;
                //ERROR
                default:
                    if (lastChar == '"') {
                        charNumber++;
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return LEX_ERROR;
                    }

                    strAddChar(token->value, c);
                    charNumber++;
                    break;
                }
                if (state != S_MULTILINE_STRING_EXIT) { lastChar = c; } 
                break;
            
            case S_MULTILINE_STRING_EXIT:
                switch (c) {
                case '"':
                    if (lastChar == '"') {
                        strAddChar(token->value, c);
                        charNumber++;
                        state = S_START;
                        token->tokenType = T_MULTILINE_STRING;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return LEX_OK;
                    }
                    strAddChar(token->value, c);
                    charNumber++;
                    lastChar = c;
                    break;
                default:
                    charNumber++;
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                    break;
                }
                break;
            
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
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return LEX_ERROR;
                    break;
                }
                break;
        }
    }
    return LEX_ERROR;
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
