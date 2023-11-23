#include "str.h"
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#pragma once

#define LEX_ERROR 1
#define LEX_OK 0


typedef struct {
    int lineNumber;
    int charNumber;
} positionInfo;

enum tokenType {
    T_NO_TOKEN, //neznamena nic, je jen placeholder pro 0
    T_WHITESPACE, //nevyužit
    T_EOF, //nevyužit
    T_EOL, // EOL
    T_IDENTIFIER, //variable

    //BRACKETS
    T_LEFT_BRACKET, // (
    T_RIGHT_BRACKET, // )
    T_LEFT_CURLY_BRACKET, // {
    T_RIGHT_CURLY_BRACKET, // }

    //OPERATORS
    T_PLUS, // +
    T_MINUS, // -
    T_MULTIPLICATION, // *
    T_DIVISION, // /
    T_LESS, // <
    T_LESS_EQUAL, // <=
    T_GREATER, // >
    T_GREATER_EQUAL, // >=
    T_EQUAL, // ==
    T_NOT_EQUAL, // !=
    T_NIL_OP, // ??

    //VARIABLES
    T_ASSIGNMENT, // =
    T_INCREMENT, // +=
    T_DECREMENT, // -=
    T_VAR_MUL_VAR, // *=
    T_VAR_DIV_VAR, // /=
    T_NOT, // !

    //DATA TYPES
    T_INT, // Integer
    T_DOUBLE, // Double
    T_STRING, // String
    T_MULTILINE_STRING, // """ """
    T_NULLABLE, // ?

    //OTHERS
    T_SEMICOLON, // ;
    T_COLON, // :
    T_COMMA, // ,
    T_ARROW, // ->
    T_END, // $
    T_E, // E in expression parser

    //KEYWORDS
    KW_DOUBLE, // Double
    KW_FUNC, // func
    KW_RETURN, // return
    KW_INT, // Int
    KW_STRING, // String
    KW_NIL, // nil
    KW_ELSE, // else
    KW_IF, // if
    KW_WHILE, // while
    KW_VAR, // var
    KW_LET, // let
    KW_UNDERSCORE, // _
    KW_READSTRING, // readString
    KW_READINT, // readInt
    KW_READDOUBLE, // readDouble
    KW_WRITE, // write
    KW_INT_TO_DOUBLE, // int2Double
    KW_DOUBLE_TO_INT, // double2Int
    KW_LENGTH, // lenght
    KW_SUBSTRING, // substring
    KW_ORD, // ord
    KW_CHR, // chr
};


enum state {
    S_START,
    S_NEW_LINE,
    S_IDENTIFIER_KW,
    S_INT,
    S_EXPONENT_INT,
    S_DOUBLE,
    S_EXPONENT_DOUBLE,
    S_STRING,
    S_STRING_ESCAPE,
    S_MULTILINE_STRING,
    S_MULTILINE_STRING_ESCAPE,
    S_MULTILINE_STRING_EXIT,
    S_MULTILINE_LINE_STRING,
    S_MULTILINE_LINE_STRING_CHECK,
    S_UNDERSCORE_IDENTIFIER,
    S_LESS_EQUAL,
    S_GREATER_EQUAL,
    S_EQUAL,
    S_NOT_EQUAL,
    S_INCREMENT,
    S_DECREMENT,
    S_VAR_MUL_VAR,
    S_VAR_DIV_VAR,
    S_NIL_OP,
    S_BLOCK_LINE_COMMENT,
    S_LINE_COMMENT,
    S_BLOCK_COMMENT,
    S_TRANSITION_TO_COMMENT,
    S_TRANSITION_TO_NEWLINE,
};

typedef struct {
    enum tokenType tokenType;
    enum tokenType tokenExpParserType; //only for expression parser
    string *value;
    positionInfo *position;
    char lastChar;
} token;

void tokenClear (token* token);
int getToken(token *token, int charNumber, int lineNumber);

const char* getTokenName(enum tokenType tokenType);