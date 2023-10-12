#include "str.h"

typedef struct {
    int lineNumber;
    int charNumber;
} positionInfo;

enum tokenType {
    T_NO_TOKEN,
    T_WHITESPACE,
    T_EOF,
    T_START,
    T_IDENTIFIER,

    //BRACKETS
    T_LEFT_BRACKET,
    T_RIGHT_BRACKET,
    T_LEFT_CURLY_BRACKET,
    T_RIGHT_CURLY_BRACKET,

    //OPERATORS
    T_PLUS,
    T_MINUS,
    T_MULTIPLICATION,
    T_DIVISION,
    T_LESS,
    T_LESS_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_EQUAL,
    T_NOT_EQUAL,
    T_NIL_OP,

    //VARIABLES
    T_ASSIGNMENT,
    T_INCREMENT,
    T_DECREMENT,
    T_VAR_MUL_VAR,
    T_VAR_DIV_VAR,
    T_NOT,

    //DATA TYPES
    T_INT,
    T_DOUBLE,
    T_STRING,
    T_MULTILINE_STRING,
    T_NULLABLE,

    //OTHERS
    T_SEMICOLON,
    T_COLON,
    T_ARROW,

    //KEYWORDS
    KW_DOUBLE,
    KW_FUNC,
    KW_RETURN,
    KW_INT,
    KW_STRING,
    KW_NIL,
    KW_ELSE,
    KW_IF,
    KW_WHILE,
    KW_VAR,
    KW_LET,
    KW_UNDERSCORE,
    KW_READSTRING,
    KW_READINT,
    KW_READDOUBLE,
    KW_WRITE,
    KW_INT_TO_DOUBLE,
    KW_DOUBLE_TO_INT,
    KW_LENGTH,
    KW_SUBSTRING,
    KW_ORD,
    KW_CHR,
};


enum state {
    S_START,
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
};

typedef struct {
    enum tokenType tokenType;
    string *value;
    positionInfo *position;
    char lastChar;
} token;

void tokenClear (token* token) {
    strClear(token->value);
    token->tokenType = T_NO_TOKEN;
    token->lastChar = '\0';
}

