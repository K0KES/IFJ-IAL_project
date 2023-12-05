#include "error.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "symtable.h"

list *allocatedStrings;

generator* generatorInit(){
    generator *gen = (generator *)(malloc(sizeof(generator))); 
    if(gen == NULL) return NULL;

    gen->functions = listInit();
    gen->mainCode = listInit();
    gen->parserStack = listInit();
    gen->temporary = listInit();
    allocatedStrings = listInit();
    ht_init(&gen->functionCallsTable);

    gen->counter = 1;
    gen->substringGenerated = false;

    return gen;
}

void generatorAddSubstringFunction(generator *gen){
    if(gen->substringGenerated) return;

    //Possible error when calling substring from declaration

    generatorPushStringToList(gen->functions,"LABEL $$substring");
    generatorPushStringToList(gen->functions,"PUSHFRAME");
    generatorPushStringToList(gen->functions,"DEFVAR LF@%retval");
    generatorPushStringToList(gen->functions,"DEFVAR LF@length");
    //generatorPushStringToList(gen->functions,"DEFVAR LF@zero");
    //generatorPushStringToList(gen->functions,"DEFVAR LF@one");
    generatorPushStringToList(gen->functions,"DEFVAR LF@relation");
    generatorPushStringToList(gen->functions,"DEFVAR LF@result");
    generatorPushStringToList(gen->functions,"DEFVAR LF@char");
    generatorPushStringToList(gen->functions,"DEFVAR LF@i");

    generatorPushStringToList(gen->functions,"MOVE LF@result nil@nil");
    generatorPushStringToList(gen->functions,"MOVE LF@%retval nil@nil");
    //generatorPushStringToList(gen->functions,"MOVE LF@zero int@0");
    //generatorPushStringToList(gen->functions,"MOVE LF@one int@1");
    generatorPushStringToList(gen->functions,"STRLEN LF@length LF@!1");
    generatorPushStringToList(gen->functions,"MOVE LF@i LF@!2");
    
    generatorPushStringToList(gen->functions,"LT LF@relation LF@!2 int@0");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@relation bool@true");

    generatorPushStringToList(gen->functions,"LT LF@relation LF@!3 int@0");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@relation bool@true");

    generatorPushStringToList(gen->functions,"GT LF@relation LF@!2 LF@length");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@relation bool@true");

    generatorPushStringToList(gen->functions,"EQ LF@relation LF@!2 LF@length");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@relation bool@true");

    generatorPushStringToList(gen->functions,"GT LF@relation LF@!2 LF@!3");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@relation bool@true");

    generatorPushStringToList(gen->functions,"GT LF@relation LF@!3 LF@length");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@relation bool@true");
    
    generatorPushStringToList(gen->functions,"MOVE LF@result string@");

    generatorPushStringToList(gen->functions,"LABEL $$substring$$while");
    generatorPushStringToList(gen->functions,"JUMPIFEQ $$substring$$return LF@i LF@!3");
    generatorPushStringToList(gen->functions,"GETCHAR LF@char LF@!1 LF@i");
    generatorPushStringToList(gen->functions,"CONCAT LF@result LF@result LF@char");
    generatorPushStringToList(gen->functions,"ADD LF@i LF@i int@1");
    generatorPushStringToList(gen->functions,"JUMP $$substring$$while");

    generatorPushStringToList(gen->functions,"LABEL $$substring$$return");
    generatorPushStringToList(gen->functions,"MOVE LF@%retval LF@result");
    generatorPushStringToList(gen->functions,"POPFRAME");
    generatorPushStringToList(gen->functions,"RETURN");
    
    gen->substringGenerated = true;
}

void generatorPushStringFirstToList(list *list, char *string){
    int stringLength = strlen(string) + 1;
    char *text = (char *)malloc(stringLength);
    memcpy(text,string,stringLength);
    listPushFirst(list,text);
}

void generatorPushStringToList(list *list, char *string){
    int stringLength = strlen(string) + 1;
    char *text = (char *)malloc(stringLength);
    memcpy(text,string,stringLength);
    listPushBack(list,text);
}

char* generatorPopFirstStringFromList(list *list){
    char* line = (char *)listPopFirst(list);
    listPushBack(allocatedStrings,line);
    return line;
}

char* generatorPopLastStringFromList(list *list){
    return (char *)listPopLast(list);
}

char* generatorGetFirstStringFromList(list *list){
    return (char *)listGetFirst(list);
}

char* generatorGetLastStringFromList(list *list){
    return (char *)listGetLast(list);
}

void generatorFree(generator *gen){
    if(gen == NULL) return;

    freeContentOfListAndDestroy(gen->functions);
    freeContentOfListAndDestroy(gen->mainCode);
    freeContentOfListAndDestroy(gen->parserStack);
    freeContentOfListAndDestroy(gen->temporary);

    if(allocatedStrings != NULL){
        freeContentOfListAndDestroy(allocatedStrings);
    }

    free(gen);
    gen = NULL;
}

char* generatorProccessFunctionCall(generator *gen, char* line){
    symtable *table = (symtable *)gen->table;
    ht_item_t *item = ht_search(gen->functionCallsTable,line);
    if(item != NULL){
        functionData * callData = (functionData *)item->data;
        symtableItem *owner = symtableFindSymtableItem(table,callData->callName);

        if(listLength(owner->funcData->overloadFunctions) == 0){
            return concatString(2,"CALL $",callData->callName);
        }else{
            listNode *overloadNode = owner->funcData->overloadFunctions->first;
            int i = 1;

            if(symtableCheckIfOverloadMatches(callData,owner->funcData)){
                return concatString(2,"CALL $",callData->callName);
            }

            while(overloadNode != NULL){
                if(symtableCheckIfOverloadMatches(callData,(functionData *)overloadNode->data)){
                    char *str = allocateString("empty_string");
                    sprintf(str,"%d",i);
        
                    return concatString(3,"CALL $",callData->callName,str);
                }
                i++;
                overloadNode = overloadNode->next;
            }
        }
    }else{
        return line;
    }
}

void generatorGenerateOutput(generator *gen){
    FILE *fptr = NULL;
    fptr = fopen("output", "w");
    fprintf(fptr, ".IFJcode23\n");
    fprintf(fptr, "JUMP $$main\n");

    char* line = (char *)listPopFirst(gen->functions);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->functions);
    }
    
    fprintf(fptr, "\nLABEL $$main\n");
    
    line = (char *)listPopFirst(gen->mainCode);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        fprintf(fptr, line);
        fprintf(fptr, "\n");
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->mainCode);
    }
    
    fclose(fptr);
    fptr = NULL;
}


void generatorGenerateOutputToStdOut(generator *gen){
    printf(".IFJcode23\n");
    printf("JUMP $$main\n");

    char* line = (char *)listPopFirst(gen->functions);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        printf("%s\n",line);
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->functions);
    }
    
    printf("\nLABEL $$main\n");
    
    line = (char *)listPopFirst(gen->mainCode);
    while(line != NULL){
        line = generatorProccessFunctionCall(gen,line);
        printf("%s\n",line);
        //free(line);
        line = NULL;
        line = (char *)listPopFirst(gen->mainCode);
    }
}

char * allocateString(char * original){
    char * copy = malloc(strlen(original) + 18); //Why not
    strcpy(copy, original);
    
    listPushBack(allocatedStrings,copy);
    return copy;
}

char * concatString(int num_args, ...){
    va_list args;
    va_start(args, num_args);

    int length = 0;
    for (int i = 0; i < num_args; ++i){
        char* text = va_arg(args, char *);
        length += strlen(text);
    }
    
    char *output = (char*)malloc(length + 1);

    va_start(args, num_args);

    char *dest = output;
    for (int i = 0; i < num_args; ++i) {
        char *src = va_arg(args, char *);
        while (*src)
            *dest++ = *src++;
    }
    *dest = '\0';

    va_end(args);
    
    listPushBack(allocatedStrings,output);
    return output;
}

char* generatorGenerateTempVarName(generator *gen){
    char* result = allocateString("musim_alokovat_takhle_xd");
    snprintf(result, sizeof(result), "temp%d", gen->counter);
    gen->counter++;
    return result;
}


char* replaceWord(const char* s, const char* oldW, 
                const char* newW) 
{ 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
 
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], oldW) == &s[i]) { 
            cnt++; 
 
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
 
    // Making new string of enough length 
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
 
    i = 0; 
    while (*s) { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
 
    result[i] = '\0'; 
    return result; 
} 

char* stringToAssemblyStringFormat(char* inputString) {

    char* output = allocateString(inputString);
    
    char threeQuotesAndNewLine[] = {34,34,34,10,0};
    char newLineAndThreeQuotes[] = {10,34,34,34,0};
    char singleQuote[] = {34,0};
    char singleQuoteEscape[] = {92,34,0};

    output = replaceWord(output,threeQuotesAndNewLine,"");
    output = replaceWord(output,newLineAndThreeQuotes,"");

    output = replaceWord(output,"\\n","\\010");
    output = replaceWord(output,"\\r","\\013");
    output = replaceWord(output,"\\t","\\009");
    output = replaceWord(output,"\\\\","\\092");
    output = replaceWord(output,"\n","\\010");
    output = replaceWord(output,"\r","\\013");
    
    output = replaceWord(output," ","\\032");

    output = replaceWord(output,singleQuoteEscape,singleQuote);

    if(output[0] == '"'){
        memmove(output, output+1, strlen(output));
    }
    if(output[strlen(output)-1] == '"'){
        output[strlen(output)-1] = '\0';
    }

    return output;
}

void printList(list *l){
    listNode *currentNode = l->first;
    DEBUG_PRINTF("[Generator] LIST PRINT: ");
    while(currentNode != NULL){
        char* line = (char *)(currentNode->data);
        DEBUG_PRINTF("%s ",line);
        currentNode = currentNode->next;
    }
    DEBUG_PRINTF("\n");
}

void freeContentOfListAndDestroy(list *list){
    listNode *node = list->first;

    while(node != NULL){
        if(node->data != NULL){
            free(node->data);   
            node->data = NULL;
        }
        node = node->next;
    }
    
    listDestroy(list);
    list = NULL;
}