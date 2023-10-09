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

struct hmap{
    size_t size;
    size_t bucket_count;
    size_t value_size;
    hmap_item_t buckets[];
};

struct hmap_item{
    char* key;
    void* data;
    hmap_item_t next;
};

const uint32_t hmap_hashing_seed = 1337;

//MurmurOAAT hashing algorythm
uint32_t hmap_hash(const char* str)
{
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    uint32_t h = hmap_hashing_seed;

    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

hmap_t hmap_init(size_t bucket_count, size_t value_size){
    hmap_t hmap = malloc(sizeof(struct hmap) + bucket_count * sizeof(hmap_item_t));
    if(hmap == NULL) return NULL;

    hmap->size = 0;
    hmap->bucket_count = bucket_count;
    hmap->value_size = value_size;

    for (size_t i = 0; i < bucket_count; i++)
    {
        hmap->buckets[i] = NULL;
    }

    return hmap;
}

hmap_item_t hmap_item_init(char* key, size_t value_size){
    hmap_item_t item = malloc(sizeof(struct hmap_item));
    if(item == NULL) return NULL;

    item->key = malloc(strlen(key) + 1);
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

void hmap_set(hmap_t hmap, char* key, void* data){
    const size_t hash = hmap_hash(key);
    const size_t bucket_index = hash % hmap->bucket_count;

    hmap_item_t new_item = hmap_item_init(key,hmap->value_size);
    if(new_item == NULL) {
        //Velky spatny :(
    }

    new_item->data = data;

    hmap_item_t bucket = hmap->buckets[bucket_index];
    if(bucket == NULL){
        hmap->buckets[bucket_index] = new_item;
    }else{
        while(bucket->next != NULL){
            bucket = bucket->next;
        }
        bucket->next = new_item;
    }
    hmap->size++;
}

void* hmap_get(hmap_t hmap, char* key){
    const size_t hash = hmap_hash(key);
    const size_t bucket_index = hash % hmap->bucket_count;

    size_t current_bucket_index = bucket_index;
    hmap_item_t current_bucket = hmap->buckets[bucket_index];
    while(current_bucket_index == bucket_index && current_bucket != NULL){
        if(!strcmp(key,current_bucket->key)){
            //return it 
            return current_bucket->data;
        }

        if(current_bucket->next != NULL){
            current_bucket = current_bucket->next;
            continue;
        }
        
        for (size_t i = current_bucket_index + 1; i < hmap->bucket_count; i++)
        {
            if((current_bucket = hmap->buckets[i]) != NULL){
                current_bucket_index = i;
                continue;
            }
        }
    }

    return NULL;
}

void main(){
    hmap_t my_hmap = hmap_init(56,sizeof(int));

    int x = 27;
    int y = 50;
    int z = 100;

    hmap_set(my_hmap,"test1",(void *)&x);
    hmap_set(my_hmap,"test2",(void *)&y);
    hmap_set(my_hmap,"test3",(void *)&z);
    
    
    int* int_ptr = (int *)hmap_get(my_hmap,"test1");
    printf("%d\n",*int_ptr);

    int_ptr = (int *)hmap_get(my_hmap,"test2");
    printf("%d\n",*int_ptr);

    int_ptr = (int *)hmap_get(my_hmap,"test3");
    printf("%d\n",*int_ptr);

    return 0;
}