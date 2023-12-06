#pragma once
#include "list.h";
//hlavickovy soubor pro praci s nekonecne dlouhymi retezci
#pragma once

typedef struct
{
  char* str;
  list *listData;
} string;


/// @brief Initializes str
/// @return pointer to str
string *strInit();

/// @brief Free
/// @param s pointer to str
void strFree(string *s);

/// @brief Clears str
/// @param s pointer str
void strClear(string *s);

/// @brief Adds char
/// @param s1 pointer to str
/// @param c char
/// @return return code
int strAddChar(string *s1, char c);

/// @brief Copies content of s1 to s2
/// @param s1 pointer to str1
/// @param s2 pointer to str2
/// @return return code
int strCopyString(string *s1, string *s2);

/// @brief Compares content of s1 to s2
/// @param s1 pointer to str1
/// @param s2 pointer to str2
/// @return return code
int strCmpString(string *s1, string *s2);

/// @brief Compares content of s1 to string
/// @param s1 pointer to str1
/// @param s2 string
/// @return return code
int strCmpConstStr(string *s1, char *s2);

/// @brief Gets string from str
/// @param s pointer to str
/// @return string
char *strGetStr(string *s);

/// @brief Updates cached string
/// @param s pointer to str
/// @return string
void strUpdateStr(string *s);

/// @brief Returns whole string
/// @param s pointer to str
/// @return string
char* strCreateStr(string *s);

/// @brief Sets whole string
/// @param s pointer to str
void strSetString(string *s, char* text);

/// @brief Get string length
/// @param s pointer to str
/// @return length
int strGetLength(string *s);

/// @brief Prints string
/// @param s pointer to str
void strPrint (string *s);