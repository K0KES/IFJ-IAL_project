/**
 * @file hashmap.c
 * @author Vít Slavíček
 * @brief Hashmap implementation in C
 * @date 9.10.2023
 * 
*/

#include "hashmap.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

struct hashmap{
    size_t size;
    size_t bucket_count;
    size_t value_size;
    hashmap_item_t buckets[];
};

struct hashmap_item{
    char* key;
    void* data;
    hashmap_item_t next;
};

const uint32_t hashmap_hashing_seed = 1337;

//MurmurOAAT hashing algorythm
uint32_t hashmap_hash(const char* str)
{
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    uint32_t h = hashmap_hashing_seed;

    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

hashmap_t hashmap_init(size_t bucket_count, size_t value_size){
    hashmap_t hashmap = (hashmap_t)(malloc(sizeof(struct hashmap) + bucket_count * sizeof(hashmap_item_t)));
    if(hashmap == NULL) return NULL;

    hashmap->size = 0;
    hashmap->bucket_count = bucket_count;
    hashmap->value_size = value_size;

    for (size_t i = 0; i < bucket_count; i++)
    {
        hashmap->buckets[i] = NULL;
    }

    return hashmap;
}

hashmap_item_t hashmap_item_init(char* key, size_t value_size){
    hashmap_item_t item = (hashmap_item_t)(malloc(sizeof(struct hashmap_item)));
    if(item == NULL) return NULL;

    item->key = (char*)(malloc(strlen(key) + 1));
    if(item->key == NULL){
        free(item);
        return NULL;
    }

    item->data = malloc(value_size);
    if(item->data == NULL){
        free((void *)item->key);
        free(item);
        return NULL;
    }

    memcpy((void *)item->key,key,strlen(key) + 1);
    memset(item->data,0,value_size);
    
    return item;
}

void hashmap_set(hashmap_t hashmap, char* key, void* data){
    const size_t hash = hashmap_hash(key);
    const size_t bucket_index = hash % hashmap->bucket_count;

    hashmap_item_t new_item = hashmap_item_init(key,hashmap->value_size);
    if(new_item == NULL) {
        //Velky spatny :(
    }

    new_item->data = data;

    hashmap_item_t bucket = hashmap->buckets[bucket_index];
    if(bucket == NULL){
        hashmap->buckets[bucket_index] = new_item;
    }else{
        while(bucket->next != NULL){
            bucket = bucket->next;
        }
        bucket->next = new_item;
    }
    hashmap->size++;
}

void* hashmap_get(hashmap_t hashmap, char* key){
    const size_t hash = hashmap_hash(key);
    const size_t bucket_index = hash % hashmap->bucket_count;

    size_t current_bucket_index = bucket_index;
    hashmap_item_t current_bucket = hashmap->buckets[bucket_index];
    while(current_bucket_index == bucket_index && current_bucket != NULL){
        if(!strcmp(key,current_bucket->key)){
            //return it 
            return current_bucket->data;
        }

        if(current_bucket->next != NULL){
            current_bucket = current_bucket->next;
            continue;
        }
        
        for (size_t i = current_bucket_index + 1; i < hashmap->bucket_count; i++)
        {
            if((current_bucket = hashmap->buckets[i]) != NULL){
                current_bucket_index = i;
                continue;
            }
        }
    }

    return NULL;
}
/*
void main(){
    hashmap_t my_hmap = hashmap_init(56,sizeof(int));

    int x = 27;
    int y = 50;
    int z = 100;

    hashmap_set(my_hmap,"test1",(void *)&x);
    hashmap_set(my_hmap,"test2",(void *)&y);
    hashmap_set(my_hmap,"test3",(void *)&z);
    
    
    int* int_ptr = (int *)hmap_get(my_hmap,"test1");
    printf("%d\n",*int_ptr);

    int_ptr = (int *)hmap_get(my_hmap,"test2");
    printf("%d\n",*int_ptr);

    int_ptr = (int *)hmap_get(my_hmap,"test3");
    printf("%d\n",*int_ptr);

    return 0;
}*/