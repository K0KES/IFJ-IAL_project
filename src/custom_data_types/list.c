#include "list.h"

#include <string.h>
#include <stdbool.h>

struct list_node{
    void *data;
    list_node_t next;
};

struct list
{
    list_node_t head;
    list_node_t tail;
    size_t size;
};

list_t list_init(size_t size){
    list_t list = malloc(sizeof(struct list));
    if(list == NULL) return NULL;

    list->head = NULL;
    list->tail = NULL;
    list->size = size;
    return list;
}

void list_clear(list_t list){
    for (size_t i = 0; i < list->size; i++)
    {
        list_pop(list);
    }
    
}

void list_free(list_t list){
    list_clear(list);
    free(list);
}

bool list_push(list_t list, void *data){
    list_node_t new_node = malloc(sizeof(struct list_node));
    if(new_node == NULL) return false;

    if(list->head == NULL){
        list->head = new_node;
        list->tail = new_node;
    }else{
        list->tail->next = new_node;
        list->tail = new_node;
    }

    new_node->data = data;
    list->size ++;

    return true;
}

bool list_pop(list_t list){
    if(list->size == 0) return false;

    list_node_t current_node = list->head;
    while(current_node != list->tail){
        current_node = current_node->next;
    }
    current_node->next = NULL;
    free(list->tail);
    list->size--;
    return true;
}

void* list_get_item(list_t list, int index){
    int i = 0;
    list_node_t current_node = list->head;
    while(current_node != NULL){
        if(i == index){
            return current_node->data;
        }
        current_node = current_node->next;
        i++;
    }
    return NULL;
}
/*
void main(){
    list_t my_list = list_init(0);

    int x = 5;
    int y = 27;

    list_push(my_list,(void *)&x);
    list_push(my_list,(void *)&y);
    
    list_pop(my_list);

    for (size_t i = 0; i < my_list->size; i++)
    {
        int* int_ptr = (int *)list_get_item(my_list,i);
        printf("%d\n",*int_ptr);
    }
    
    return 0;
}*/