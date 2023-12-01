#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "../../src/scanner.h"

//gcc src/error.c  src/str.c src/scanner.c -o scanner tests/scanner_tests/main.c -o main
// ./main scanner_test.txt

int main(int argc, char const *argv[])
{   
    token *token_e = malloc(sizeof(token));
    token_e->value = (string*)malloc(sizeof(string));
    strInit(token_e->value);
    token_e->position = (positionInfo*)malloc(sizeof(positionInfo));
    token_e->position->charNumber = 0;
    token_e->position->lineNumber = 1;   

    int tokenReturnValue;  
    while (tokenReturnValue = getToken(token_e,  token_e->position->charNumber, token_e->position->lineNumber) != LEX_ERROR) {
        printf("'");
        printf("\ntoken type:%d, value:", token_e->tokenType);
        strPrint(token_e->value);
        printf(", lastChar='%c', lineNumber=%d, charNumber=%d", token_e->lastChar, token_e->position->lineNumber, token_e->position->charNumber);
        printf("\n\n");
        tokenClear(token_e);
    };
    //EOF
    if (token_e->tokenType == T_EOF) {
        // printf("'");
        printf("\ntoken type:%d, value:", token_e->tokenType);
        strPrint(token_e->value);
        printf(", lastChar='%c', lineNumber=%d, charNumber=%d", token_e->lastChar, token_e->position->lineNumber, token_e->position->charNumber);
        printf("\n\n");
        tokenClear(token_e);
    }

}
