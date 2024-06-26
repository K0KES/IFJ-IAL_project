#include "list.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

list *listInit(){
    list *l = (list *)(malloc(sizeof(list)));
    if(l == NULL) return NULL;

    l->first = NULL;
    l->last = NULL;
    l->size = 0;
    return l;
}

int listLength(list *list){
    return list->size;
}

listNode *listInitNode(void *data){
    listNode *node = (listNode *)malloc(sizeof(listNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void listClear(list *list){
    listNode *currentNode = list->first;
    listNode *nextNode;

    while(currentNode != NULL){
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }

    list->first = NULL;
    list->last = NULL;
    list->size = 0;
}

void listDestroy(list *list){
    listNode *currentNode = list->first;
    listNode *nextNode;

    while(currentNode != NULL){
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }

    free(list);
}

bool listPushFirst(list *list, void *data){
    listNode *newNode = listInitNode(data);

    if(list->size == 0){
        list->last = newNode;
    }else{
        list->first->prev = newNode;
        newNode->next = list->first;
    }

    list->first = newNode;
    list->size++;

    return true;
}

bool listPushBack(list *list, void *data) 
{
  listNode *newNode = listInitNode(data);

  if(list->size == 0) {
    list->first = newNode;
  } else {
    list->last->next = newNode;
    newNode->prev = list->last;
  }
  list->last = newNode;
  list->size++;
  
  return true;
}

void* listPopFirst(list *list){
    if(!list) return NULL;

    listNode *node = list->first;
    if(node == NULL) return NULL;

    void *data = node->data;

    list->first = node->next;

    list->size--;

    free(node);
    node = NULL;

    return data;
}

void* listPopLast(list *list){
    if(!list) return NULL;

    listNode *node = list->last;
    if(node == NULL) return NULL;

    void *data = node->data;
    
    if(node->prev == NULL) 
        list->first = node->next;
    else 
        node->prev->next = node->next;
    
    if(node->next == NULL)
        list->last = node->prev; 
    else
        node->next->prev = node->prev;

    list->size--;

    free(node);

    return data;
}

void* listGetFirst(list *list) {
    if(list->size == 0) return NULL;
    return list->first->data;
}

void* listGetLast(list *list) {
    if(list->size == 0) return NULL;
    return list->last->data;
}


void *listGet(list *list, int index){
    listNode *node = list->first;
    int i = 0;
    while(node != NULL){
        if(i == index){
            return node->data;
        }

        i++;
        node = node->next;
    }

    return NULL;
}
