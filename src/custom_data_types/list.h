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

list *listInit();
int listLength(list *list);

void* listGetLast(list *list);
void* listGetFirst(list *list);

bool listPushFirst(list *list, void *data);
bool listPushBack(list *list, void *data);
void* listPopFirst(list *list);

void listClear(list *list);
void listDestroy(list *list);