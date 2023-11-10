#include "symtable.h"
#include "scanner.h"

void symtableInit(symtable *table){
    table = (symtable *)(malloc(sizeof(symtable))); 
    if(table == NULL) return;

    table->tables = listInit();
    if(table->tables == NULL){ free(table); return; }
    
    
    if(!symtableEnterScope(table,NULL)){
        return;
    }
}

bool symtableEnterScope(symtable *table,char* scope){

    ht_table_t hashmap;
    ht_init(&hashmap);
    if(hashmap == NULL) return false;
    
    listPushFirst(table->tables,&hashmap);

    if(scope != NULL){
        int stringLength = strlen(scope) + 1;
        char *string = (char *)malloc(stringLength);
        memcpy(string,scope,stringLength);

        listPushFirst(table->scopes,&string);
    }
}

void symtableExitScope(symtable *table){
    ht_table_t **hashmap;
    (*hashmap) = (ht_table_t *)listGetLast(table->tables);
    return;
    //ht_delete_all(hashmap);
    //listPop(table->tables);
    /*
    if(listLength(table->scopes) != 0){
        char **scopeString = (char **)listGetLast(table->scopes);
        free(scopeString);
        listPop(table->scopes);
    }*/

}

void symtableFree(symtable *table){
    
    while(listLength(table->tables) != 0){
        symtableExitScope(table);
    }

    listDestroy(table->tables);
    listDestroy(table->scopes);
    free(table);
}
/*

void symtableInsert(symtable symtable, token token, symbolType symbolType){
    return;
}

void symtableExitScope(symtable symtable){
    return;
}

bool symtableContainsSymbol(symtable symtable){
    return false;
}

bool symtableContainsFunction(symtable symtable){
    return false;
}

bool symtableContainsVariable(symtable symtable){
    return false;
}

void symtableFree(symtable symtable){
    return;
}*/