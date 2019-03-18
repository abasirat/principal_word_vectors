/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 * Part of the code is inspired by the implementation of GloVe https://nlp.stanford.edu/projects/glove/
 *
 **/

/*    
   Main code: J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. 

   Modified by: Ali Basirat August 2017
*/

#define _GNU_SOURCE

#ifndef __HASH_H__
#define __HASH_H__


#define TSIZE 1048576
#define SEED  1159241
#define HASHFN  bitwisehash
#define STRCMP  scmp

#include "basic_header.h"

#include <string.h>

typedef enum {HASH_DESCEND = 0, HASH_ASCEND = 1} hash_sort_type ;

typedef struct hashrec{
  char  *key;
  hash_value_type value ;
  struct hashrec *next;
} hashrec_type;

typedef hashrec_type** hash_table ;

/* Create hash table, initialise ptrs to NULL */
hash_table inithashtable() ;

/* Search hash table for given string, return record if found, else NULL */
hashrec_type* hashsearch(hash_table ht, char *key) ;

/* Search hash table for given string, insert if not found */
hashrec_type* hashinsert(hash_table ht, char *key) ;

/* return number of (key,value) pairs kept by the input hash*/
size_t hashgetsize(hash_table ht) ;

/* delete the hash content and free its pointer */
void hashclear(hash_table* ht) ;

hash_table hashduplicate(hash_table ht) ;

hash_value_type hashprune(hash_table ht, hash_value_type thr) ;
  
hash_table hashgetrecords(hash_table ht, hash_sort_type adescend) ;
 
#endif
