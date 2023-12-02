#pragma once
#include "list.h";
//hlavickovy soubor pro praci s nekonecne dlouhymi retezci
#pragma once

typedef struct
{
  list *listData;
} string;


string *strInit();
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
void strSetString(string *s, char* text);
int strGetLength(string *s);

void strPrint (string *s);
