#include <stdbool.h>
#include <stdlib.h>

typedef struct list *list_t;
typedef struct list_node *list_node_t;

list_t list_init(size_t size);
size_t list_length(list_t list);
void list_clear(list_t list);
void list_free(list_t list);
bool list_push(list_t list, void *data);
bool list_pop(list_t list);
void* list_get_item(list_t list, int index);