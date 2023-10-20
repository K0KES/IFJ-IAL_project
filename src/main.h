#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"

struct programState {
    token *lastReadToken;
    bool isLastReadTokenValid;
    unsigned int lineNumber;
    unsigned int charNumber;
} programState;



int main(int argc, char const *argv[]);


