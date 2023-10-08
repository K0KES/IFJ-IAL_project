#include "str.h"

typedef struct {
    int lineNumber;
    int charNumber;
} positionInfo;

enum tokenType {
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
    T_NULLABLE,

    //OTHERS
    T_SEMICOLON,
    T_COLON,

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
    KW_UNDERSCORE
};


enum state {
    S_START,
    S_IDENTIFIER_KW,
    S_INT,
    S_DOUBLE,
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
} token;

