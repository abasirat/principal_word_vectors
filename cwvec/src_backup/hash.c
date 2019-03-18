/*    
   Main code: J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. 

   Modified by: Ali Basirat August 2017
*/

#ifndef __HASH_C__
#define __HASH_C__

#include "hash.h"

static int scmp( char *s1, char *s2 ){
  while( *s1 != '\0' && *s1 == *s2 ){s1++;s2++;}
  return( *s1-*s2 );
}

static int comparehashrec_type_ascend(const void* a, const void* b) {
  hash_value_type c ;
  if ( (c=(*(hash_table)a)->value - (*(hash_table)b)->value) != 0) return (c>0 ? 1 : -1);
  else return (STRCMP((*(hash_table)a)->key,(*(hash_table)b)->key)) ;
}

static int comparehashrec_type_descend(const void* a, const void* b) {
  hash_value_type c ;
  if ( (c=(*(hash_table)a)->value - (*(hash_table)b)->value) != 0) return (c>0 ? -1 : 1);
  else return (STRCMP((*(hash_table)a)->key,(*(hash_table)b)->key)) ;
}


static unsigned int bitwisehash(char *key){
  char  c;
  unsigned int h = SEED ;
  for( ; ( c=*key )!='\0' ; key++ ) h ^= ( (h << 5) + c + (h >> 2) );
  return((unsigned int)((h&0x7fffffff) % TSIZE));
}

/* Create hash table, initialise ptrs to NULL */
hash_table inithashtable(){
  int		i;
  hash_table    ht;

  ht = (hash_table) malloc( sizeof(hashrec_type *) * TSIZE );

  for(i=0 ; i<TSIZE ; i++) ht[i] = (hashrec_type *) NULL;

  return(ht);
}

/* Search hash table for given string, return record if found, else NULL */
hashrec_type* hashsearch(hash_table ht, char *key){
  hashrec_type	*htmp, *hprv;
  unsigned int hval = HASHFN(key);

  for( hprv = NULL, htmp=ht[hval]
      ; htmp != NULL && STRCMP(htmp->key, key) != 0
      ; hprv = htmp, htmp = htmp->next ) {}

  /* move to front on access */  
  if( hprv!=NULL && htmp!=NULL) {
    hprv->next = htmp->next;
    htmp->next = ht[hval];
    ht[hval] = htmp;
  }
  return(htmp);
}

/* Search hash table for given string, insert if not found */
hashrec_type* hashinsert(hash_table ht, char *key){
  hashrec_type	*htmp, *hprv;
  unsigned int hval = HASHFN(key);

  for( hprv = NULL, htmp=ht[hval]
        ; htmp != NULL && STRCMP(htmp->key, key) != 0
        ; hprv = htmp, htmp = htmp->next ) ;

  if( htmp==NULL ){
    if ( !(htmp = (hashrec_type *) malloc( sizeof(hashrec_type) )) )
      return NULL ;
    htmp->key = strdup(key) ;
    htmp->value = 0 ;
    htmp->next = NULL;
    if( hprv==NULL ) ht[hval] = htmp;
    else hprv->next = htmp;
    /* new records are not moved to front */
  }
  else{
    /* move to front on access */
    if( hprv!=NULL ) {
      hprv->next = htmp->next;
      htmp->next = ht[hval];
      ht[hval] = htmp;
    }
  }
  return(htmp);
}

/* return number of (key,value) pairs kept by the input hash*/
size_t hashgetsize(hash_table ht) {
  hashrec_type* htmp ;
  unsigned int i, sz = 0;
  for (i=0 ; i<TSIZE ; i++) 
    for (htmp=ht[i] ; htmp != NULL; htmp = htmp->next ) 
      sz++;
  return sz ;
}

/* delete the hash content and free its pointer */
void hashclear(hash_table* ht) {
  hashrec_type *htmp, *hprv;
  unsigned int i ;

  if (ht == NULL) return ;

  for (i=0 ; i<TSIZE ; i++) 
    for(hprv = NULL, htmp=(*ht)[i]; htmp!=NULL ; hprv=htmp, htmp=htmp->next)
      if (hprv!=NULL) {free(hprv->key) ; free(hprv) ;}
  free(*ht) ;
  *ht = NULL ;
  return ;
}

hash_value_type hashprune(hash_table ht, hash_value_type thr) {
  hashrec_type *hprv, *htmp, *hnxt;
  unsigned int i ;
  hash_value_type n = 0 ; 
  for (i=0 ; i<TSIZE ; i++) {
    for (hprv = NULL, htmp = ht[i] ; htmp!=NULL ; ) {
      hnxt = htmp->next ;
      if (htmp->value < thr) {
        n += htmp->value ;
        free(htmp->key) ; free (htmp) ;
        if (!hprv) ht[i] = hnxt ;
        else hprv->next = hnxt ;
        htmp = hnxt ;
      }else {
        hprv = htmp ;
        htmp = hnxt ;
      }
    }
  }
  return n ;
}

hash_table hashgetrecords(hash_table ht, hash_sort_type adescend) {
  unsigned int i, j=0 ;
  size_t sz = hashgetsize(ht) ;
  hashrec_type *htmp, **array = (hash_table)malloc(sizeof(hashrec_type*)*sz) ;
  for (i=0 ; i<TSIZE ; i++) 
    for (htmp=ht[i] ; htmp != NULL; htmp = htmp->next ) 
      array[j++] = htmp ;

  if (adescend == HASH_ASCEND) 
    qsort(array, sz, sizeof(hashrec_type*), comparehashrec_type_ascend) ;
  else if (adescend == HASH_DESCEND) 
    qsort(array, sz, sizeof(hashrec_type*), comparehashrec_type_descend) ;

  return array ;
}

hash_table hashduplicate(hash_table ht) {
  unsigned int i ;
  hashrec_type	*htmp, *hdup;
  hash_table htd = inithashtable() ;

  for (i=0 ; i<TSIZE ; i++) {
    for (htmp=ht[i] ; htmp != NULL; htmp = htmp->next) {
      hdup = hashinsert(htd, htmp->key) ;
      hdup->value = htmp->value ;
    }
  }

  return htd ;
}
#endif
