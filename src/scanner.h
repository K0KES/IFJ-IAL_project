#include "str.h"
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "error.h"
#pragma once

#define LEX_ERROR 1
#define LEX_OK 0

//for debugging purposes only
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
    T_INT = 120 , // Integer
    T_DOUBLE = 121, // Double
    T_STRING = 122, // String
    T_BOOL = 123, // Boolean
    T_VOID = 124, // Void only for symtable
    T_NULLABLE = 125, // ?
    KW_NIL = 126, // nil
    T_MULTILINE_STRING, // """ """

    //OTHERS
    T_SEMICOLON = 40, // ;
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
    S_STRING_HEXA,
    S_STRING_ESCAPE,
    S_MULTILINE_STRING,
    S_MULTILINE_STRING_HEXA,
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
    bool is_return_from_func; //only for expression parser
    bool is_nullable; //only for expression parser  
    bool is_number_literal; //only for expression parser

} token;

/// @brief function for cleaning token
/// @param token token that will be cleared
void tokenClear (token* token);

/// @brief function for handeling stdin and creating tokens
/// @param token where to save token
/// @param charNumber current charnumber
/// @param lineNumber current linenumber
int getToken(token *token, int charNumber, int lineNumber);


/// @brief function for checking errors in Multiline string and also check for errors
/// @param multiLine string that has to be changed by this func
string* multilineStringCheck (string* multiLine);

/// @brief get token name by Enum
/// @param tokenType a enum of token
const char* getTokenName(enum tokenType tokenType);