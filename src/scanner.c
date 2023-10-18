#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include "scanner.h"

void tokenClear (token* token) {
    strClear(token->value);
    token->tokenType = T_NO_TOKEN;
    token->lastChar = '\0';
}

int getToken(token *token, int charNumber, int lineNumber, FILE *source) {
    char c;
    enum state state = S_START;
    printf("'");
    char lastChar = '\0';
    while (c != EOF) {
        c = getc(source);
        if (state != S_BLOCK_COMMENT && state != S_LINE_COMMENT && state != S_BLOCK_LINE_COMMENT && c != '/') { printf("%c", c); }
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
                        printf("\nEOL\n");
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
                        return 0;
                        break;
                    case '(':
                        token->tokenType = T_LEFT_BRACKET;
                        strAddChar(token->value, c);
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                        break;
                    case ')':
                        token->tokenType = T_RIGHT_BRACKET;
                        strAddChar(token->value, c);
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                        break;
                    case '{':
                        token->tokenType = T_LEFT_CURLY_BRACKET;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return 0;
                        break;
                    case '}':
                        token->tokenType = T_RIGHT_CURLY_BRACKET;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return 0;
                        break;
                    case ':':
                        token->tokenType = T_COLON;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return 0;
                        break;
                    case ';':
                        token->tokenType = T_SEMICOLON;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return 0;
                        break;
                    case ',':
                        token->tokenType = T_COLON;
                        token->lastChar = lastChar;
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        strAddChar(token->value, c);
                        return 0;
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
            case S_INCREMENT:
                if (c == '+') { 
                    token->tokenType = T_INCREMENT;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_PLUS;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                break;

             case S_DECREMENT:
                if (c == '-') { 
                    token->tokenType = T_DECREMENT;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else if (c == '>') {
                    token->tokenType = T_ARROW;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_MINUS;
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    ungetc(c, source);
                    return 0;
                }
                break;

             case S_LESS_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_LESS_EQUAL;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_LESS;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                break;

             case S_GREATER_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_GREATER_EQUAL;
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_GREATER;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                break;
            
            case S_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_EQUAL; 
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_ASSIGNMENT;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                break;

            case S_NIL_OP:
                if (c == '?') { 
                    token->tokenType = T_NIL_OP; 
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_NULLABLE;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                break;
            case S_NOT_EQUAL:
                if (c == '=') { 
                    token->tokenType = T_NOT_EQUAL; 
                    charNumber++;
                    strAddChar(token->value, c);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
                }
                else {
                    token->tokenType = T_NOT;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
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
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "else") == 0) {
                        token->tokenType = KW_ELSE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "func") == 0) {
                        token->tokenType = KW_FUNC;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "if") == 0) {
                        token->tokenType = KW_IF;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "Int") == 0) {
                        token->tokenType = KW_INT;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "let") == 0) {
                        token->tokenType = KW_LET;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "nil") == 0) {
                        token->tokenType = KW_NIL;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "return") == 0) {
                        token->tokenType = KW_RETURN;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "String") == 0) {
                        token->tokenType = KW_STRING;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "var") == 0) {
                        token->tokenType = KW_VAR;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "while") == 0) {
                        token->tokenType = KW_WHILE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "readString") == 0) {
                        token->tokenType = KW_READSTRING;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "readInt") == 0) {
                        token->tokenType = KW_READINT;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "readDouble") == 0) {
                        token->tokenType = KW_READDOUBLE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "write") == 0) {
                        token->tokenType = KW_WRITE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "Int1Double") == 0) {
                        token->tokenType = KW_INT_TO_DOUBLE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "Double1Int") == 0) {
                        token->tokenType = KW_DOUBLE_TO_INT;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "length") == 0) {
                        token->tokenType = KW_LENGTH;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "substring") == 0) {
                        token->tokenType = KW_SUBSTRING;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "ord") == 0) {
                        token->tokenType = KW_ORD;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else if (strCmpConstStr(token->value, "chr") == 0) {
                        token->tokenType = KW_CHR;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    else {
                        token->tokenType = T_IDENTIFIER;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                    return 1;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if (lastChar == 'e' || lastChar == 'E' || lastChar == '+' || lastChar == '-') {
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return 1;    
                    }
                    token->tokenType = T_INT;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
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
                        return 1;
                    }
                    else {
                        token->tokenType = T_INT;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                        return 1;
                    }
                    else {
                        token->tokenType = T_INT;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
                    }
                    break;
                break;
             case S_EXPONENT_DOUBLE:
                switch (c) {
                case 'e':
                case 'E':
                case '.':
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return 1;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if (lastChar == 'e' || lastChar == 'E' || lastChar == '+' || lastChar == '-') {
                        printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                        return 1;    
                    }
                    token->tokenType = T_DOUBLE;
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
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
                        return 1;
                    }
                    else {
                        token->tokenType = T_DOUBLE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                        return 1;
                    }
                    else {
                        token->tokenType = T_DOUBLE;
                        ungetc(c, source);
                        token->position->charNumber = charNumber;
                        token->position->lineNumber = lineNumber;
                        return 0;
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
                    printf("%c", lastChar);
                    ungetc(c, source);
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
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
                    return 1;
                    break;
                default:
                    ungetc(c, source);
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
                    return 0;
                    break;
                case '\r':
                case '\n':
                case EOF:
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return 1;
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
                    return 1;
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
                    ungetc(c, source);
                    state = S_START;
                    token->tokenType = T_STRING;
                    token->position->charNumber = charNumber;
                    token->position->lineNumber = lineNumber;
                    return 0;
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
                        return 1;
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
                        return 0;
                    }
                    strAddChar(token->value, c);
                    charNumber++;
                    lastChar = c;
                    break;
                default:
                    charNumber++;
                    printf("\n\nChyba na radku: %d, znak: %d\n\n", lineNumber, charNumber);
                    return 1;
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
                    return 1;
                    break;
                }
                break;
        }


    }
    return 1;
}
