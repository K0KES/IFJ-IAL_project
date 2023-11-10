/**
 * @file hashtable.c
 * @author Vít Slavíček
 * @brief Hashtable implementation in C from IAL
 * @date 9.10.2023
 * 
*/

/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int HT_SIZE = MAX_HT_SIZE;
const int hashmap_hashing_seed = 1337;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    int h = hashmap_hashing_seed;

    for (; *key; ++key) {
        h ^= *key;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
  //table = (ht_table_t *)malloc(sizeof(ht_table_t));
  
  //table = malloc(HT_SIZE * sizeof(struct ht_item));
  //if(table == NULL) return;

  for (size_t i = 0; i < HT_SIZE; i++)
  {
      (*table)[i] = NULL;
  }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  const int hash = get_hash(key);

  ht_item_t *currentItem = (*table)[hash];

  while(currentItem != NULL){
      if(strcmp(key,currentItem->key) == 0){
          return currentItem;
      }
      currentItem = currentItem->next;
  }

  return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  ht_item_t *currentItem = ht_search(table,key);
  if(currentItem == NULL){
    const int hash = get_hash(key);

    ht_item_t *newItem = (ht_item_t *)malloc(sizeof(ht_item_t));
    newItem->key = key;
    newItem->value = value;
    newItem->next = NULL;

    if((*table)[hash] != NULL){
      newItem->next = (*table)[hash];
    }

    (*table)[hash] = newItem;

  }else{
    currentItem->value = value;
  }
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  ht_item_t *currentItem = ht_search(table,key);
  if(currentItem == NULL) return NULL;
  return &(currentItem->value);
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  const int hash = get_hash(key);

  ht_item_t *currentItem = (*table)[hash];
  ht_item_t *lastItem = NULL;

  while(currentItem != NULL){
      if(strcmp(key,currentItem->key) == 0){
          if(lastItem != NULL){
            lastItem->next = currentItem->next;
          }
          
          free(currentItem);
          currentItem = NULL;
          return;
      }
      lastItem = currentItem;
      currentItem = currentItem->next;
      
  }

  return;
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
  for (size_t i = 0; i < HT_SIZE; i++)
  {
    ht_item_t *currentItem = (*table)[i];
    ht_item_t *nextItem = NULL;

    while(currentItem != NULL){
      nextItem = currentItem->next;
      free(currentItem);
      currentItem = nextItem;  
    }

    (*table)[i] = NULL;
    
  }
}
