#include "symtable.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

struct symtable
{
	list tables;
	hmap global_table;
};

#define HASHMAP_SIZE 800

symtable_t symtable_init(){
    symtable_t symtable = (symtable_t)(malloc(sizeof(struct symtable))); 
    if(symtable == NULL) return NULL;

    symtable->tables = list_init(sizeof(hashmap_t));
    if(symtable->tables == NULL){ free(symtable); return NULL }


}

bool symtable_enter_scope(symtable_t symtable,const char* scopename){
    hashmap_t hashmap = hashmap_init(HASHMAP_SIZE, sizeof(symbol_t)) //SYMBOL_T neexistuje
    if(hashmap == NULL) return false;

    //TODO
}

void symtable_exit_scope(symtable_t symbtable){

}

bool symtable_contains_symbol(symtable_t symtable){

}

bool symtable_contains_function(symtable_t symtable){

}

bool symtable_contains_variable(symtable_t symtable){

}

void symtable_free(symtable_t symtable){
    
}

int main(){
    return 0;
}