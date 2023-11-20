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

/*
void main(){
    list *my_list = listInit();

    int x = 5;
    int y = 27;

    
    listPushFirst(&my_list,(void *)&x);
    listPushFirst(&my_list,(void *)&y);
    
    listPushFirst(&my_list,(void *)&y);
    
    listPush(my_list,(void *)&x);
    listPush(my_list,(void *)&y);
    listPush(my_list,(void *)&x);
    listPush(my_list,(void *)&y);
    listPush(my_list,(void *)&x);
    listPush(my_list,(void *)&y);
    
    
    listPop(my_list);

    while(my_list->size != 0)
    {
        int* int_ptr = (int *)listPopFirst(&my_list);
        printf("%d\n",*int_ptr);
    }

    listDestroy(my_list);
    
    return;
}*/