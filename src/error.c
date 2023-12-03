#include "error.h"
#include <stdio.h>

void raiseError(enum errorType error){
    DEBUG_PRINTF("Error: %s\n",getErrorName(error));
    fprintf(stderr,"Error: %s\n",getErrorName(error));
    exit(error);
}

const char* getErrorName(enum errorType tokenType) {
   switch (tokenType) {
        case ERR_LEXICAL: return "ERR_LEXICAL";
        case ERR_SYNTAX: return "ERR_SYNTAX";
        case ERR_UNDEFINED_FUNCTION: return "ERR_UNDEFINED_FUNCTION";
        case ERR_WRONG_NUMBER_OF_ARGUMENTS: return "ERR_WRONG_NUMBER_OF_ARGUMENTS";
        case ERR_UNDEFINED_VARIABLE: return "ERR_UNDEFINED_VARIABLE";
        case ERR_WRONG_RETURN_TYPE: return "ERR_WRONG_RETURN_TYPE";
        case ERR_WRONG_TYPE: return "ERR_WRONG_TYPE";
        case ERR_MISSING_TYPE: return "ERR_MISSING_TYPE";
        case ERR_SEMANTIC: return "ERR_SEMANTIC";
        case ERR_INTERNAL: return "ERR_INTERNAL";
   }
   return "";
}