#ifndef ERROR_H
#define ERROR_H

enum errorType {
    ERR_LEXICAL = 1,
    ERR_SYNTAX = 2,
    ERR_UNDEFINED_FUNCTION = 3,
    ERR_WRONG_NUMBER_OF_ARGUMENTS = 4,
    ERR_UNDEFINED_VARIABLE = 5,
    ERR_WRONG_RETURN_TYPE = 6,
    ERR_WRONG_TYPE = 7,
    ERR_MISSING_TYPE = 8,
    ERR_SEMANTIC = 9,
    ERR_INTERNAL = 99
};

void raiseError(enum errorType error);

#endif