// #include <stdio.h>
// #include <ctype.h>
// #include <stdlib.h>
// #include "../../src/scanner.h"


// // gcc src/str.c src/scanner.c -o scanner tests/scanner_tests/main.c -o main
// // ./main scanner_test.txt

// int main(int argc, char const *argv[])
// {
//     token *token_e = (token*)malloc(sizeof(token));
//     strInit(token_e->value);
//     token_e->position = (positionInfo*)malloc(sizeof(positionInfo));
//     token_e->position->charNumber = 0;
//     token_e->position->lineNumber = 1;

//     FILE *f;
//     if (argc == 1) {
//         printf("Neni zadan vstupni soubor\n");
//         return 0;
//     }
//     if ((f = fopen(argv[1], "r")) == NULL) {
//         printf("Soubor se nepodarilo otevrit\n");
//         return 1;
//     }   

//     int tokenReturnValue = getToken(token_e,  token_e->position->charNumber, token_e->position->lineNumber, f);   
//     while (tokenReturnValue != 1) {
//         printf("'");
//         printf("\ntoken type:%d, value:", token_e->tokenType);
//         strPrint(token_e->value);
//         printf(", lastChar='%c', lineNumber=%d, charNumber=%d", token_e->lastChar, token_e->position->lineNumber, token_e->position->charNumber);
//         printf("\n\n");
//         tokenClear(token_e);
//         tokenReturnValue = getToken(token_e, token_e->position->charNumber, token_e->position->lineNumber, f);
//     };

// }
