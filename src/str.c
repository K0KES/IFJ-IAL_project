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

string *strInit() {
   string *s = (string *)malloc(sizeof(string));
   s->listData = listInit();
   s->str = NULL;
   strUpdateStr(s);

   return s;
}

void strFree(string *s) {
   listDestroy(s->listData);
   s->listData = NULL;
   free(s->str);
   s->str = NULL;
   free(s);
   s = NULL;
}

void strClear(string *s) {
   listClear(s->listData);
   strUpdateStr(s);
}

int strAddChar(string *s1, char c) {
   listPushBack(s1->listData,c);
   strUpdateStr(s1);
   return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2) {
   strClear(s1);

   listNode *node = s2->listData->first;
   while(node != NULL){
      strAddChar(s1,(char)node->data);
      node = node->next;
   }

   strUpdateStr(s1);

   return STR_SUCCESS;
}

char strGetCharOnIndex(string *s1, int index){
   return (char)listGet(s1->listData,index);
}

int strCmpString(string *s1, string *s2) {
   char* text1 = strGetStr(s1);
   char* text2 = strGetStr(s2);
   
   return strcmp(text1, text2);
}

int strCmpConstStr(string *s1, char* s2) {
   char* text1 = strGetStr(s1);
   return strcmp(text1, s2);
}

void strSetString(string *s, char* text){
   if(text == NULL) return;
   strClear(s);
   for (size_t i = 0; text[i] != '\0'; i++)
   {
      strAddChar(s,text[i]);
   }

   strUpdateStr(s);
}

void strUpdateStr(string *s){
   if(s->str != NULL){
      free(s->str);
      s->str = NULL;
   }
   s->str = strCreateStr(s);
}

char* strCreateStr(string *s){
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

char *strGetStr(string *s) {
   return s->str;
}

int strGetLength(string *s) {
   return listLength(s->listData);
}

void strPrint (string *s) {
   //DEBUG
}

void strpPop (string *s) {
   listPopLast(s->listData);
   strUpdateStr(s);
}
