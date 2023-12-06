#include <stdbool.h>
#include <stdlib.h>
#pragma once

typedef struct listNode_s{
    void *data;
    struct listNode_s *prev;
    struct listNode_s *next;
} listNode;

typedef struct list_s
{
    listNode *first;
    listNode *last;
    int size;
} list;

/// @brief Inits list
/// @return pointer to list
list *listInit();

/// @brief Gets list length
/// @param list pointer to list
/// @return length
int listLength(list *list);

/// @brief Get last node data
/// @param list pointer to list
/// @return void* data
void* listGetLast(list *list);

/// @brief Get first node data
/// @param list pointer to list
/// @return void* data
void* listGetFirst(list *list);

/// @brief Get node data at index
/// @param list pointer to list
/// @param index index
/// @return void* data
void *listGet(list *list, int index);

/// @brief Push first to list
/// @param list pointer to list
/// @param data pointer to data
/// @return true | false
bool listPushFirst(list *list, void *data);

/// @brief Push last to list
/// @param list pointer to list
/// @param data pointer to data
/// @return true | false
bool listPushBack(list *list, void *data);

/// @brief Pop first from list and returns data
/// @param list pointer to list
/// @return void* data
void* listPopFirst(list *list);

/// @brief Pop last from list and returns data
/// @param list pointer to list
/// @return void* data
void* listPopLast(list *list);

/// @brief Clears list
/// @param list pointer to list
void listClear(list *list);

/// @brief Frees list
/// @param list pointer to list
void listDestroy(list *list);
