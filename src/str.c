//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <string.h>
#include <stdlib.h>
#include "str.h"
#include <stdio.h>
#include "error.h"
#pragma once

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR   1
#define STR_SUCCESS 0

// funkce vytvori novy retezec
string *strInit() {
   string *s = (string *)malloc(sizeof(string));
   s->listData = listInit();

   return s;
}

// funkce uvolni retezec z pameti
void strFree(string *s) {
   listDestroy(s->listData);
   s->listData = NULL;
   free(s);
   s = NULL;
}

// funkce vymaze obsah retezce
void strClear(string *s) {
   listClear(s->listData);
}

// prida na konec retezce jeden znak
int strAddChar(string *s1, char c) {
   listPushBack(s1->listData,c);

   /*
   if (s1->length +1 >= s1->allocSize)
   {  
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL) {   
           DEBUG_PRINTF("\n\nERRORNULL\n");
         raiseError(ERR_INTERNAL);
      }
      s1->allocSize = s1->length + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';*/
   return STR_SUCCESS;
}

// prekopiruje retezec s2 do s1
int strCopyString(string *s1, string *s2) {
   strClear(s1);

   listNode *node = s2->listData->first;
   while(node != NULL){
      strAddChar(s1,(char)node->data);
      node = node->next;
   }

   /*
   int newLength = s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL) 
         raiseError(ERR_INTERNAL);
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;*/
   return STR_SUCCESS;
}

char strGetCharOnIndex(string *s1, int index){
   return (char)listGet(s1->listData,index);
}

// porovna oba retezce a vrati vysledek
int strCmpString(string *s1, string *s2) {
   char* text1 = strGetStr(s1);
   char* text2 = strGetStr(s2);

   printf("COMPRAITNG %s = %s \n",text1,text2);
   return strcmp(text1, text2);
}

// porovna nas retezec s konstantnim retezcem
int strCmpConstStr(string *s1, char* s2) {
   char* text1 = strGetStr(s1);

   //printf("COMPRAITNG %s %s = %d \n",text1,s2,strcmp(text1, s2));
   return strcmp(text1, s2);
}

void strSetString(string *s, char* text){
   strClear(s);
   for (size_t i = 0; text[i] != '\0'; i++)
   {
      strAddChar(s,text[i]);
   }
}

// vrati textovou cast retezce
char *strGetStr(string *s) {
   
   listNode *node = s->listData->first;
   char * text = (char *)malloc(listLength(s->listData)+1);

   int i = 0;
   while(node != NULL){
      text[i] = (char)node->data;
      node = node->next;
      i++;
   }

   text[i] = '\0';

   return text;
}

// vrati delku daneho retezce
int strGetLength(string *s) {
   return listLength(s->listData);
}

void strPrint (string *s) {
   /*
   int i = 0;
   while (s->str[i] != '\0') {
      DEBUG_PRINTF("%c", s->str[i]);
      i++;
   }*/
   
}

void strpPop (string *s) {
   listPopLast(s->listData);
}
