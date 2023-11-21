#include "error.h"

void raiseError(enum errorType error){
    printf("Error: %d\n",error);
    exit(error);
}