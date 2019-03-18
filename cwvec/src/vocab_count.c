/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 *
 * Part of the code is inspired by the implementation of GloVe https://nlp.stanford.edu/projects/glove/
 * 
 **/

#ifndef __VOCAB_COUNT_C
#define __VOCAB_COUNT_C

#include "hash.h"
#include "tools.h"
#include "str_norm.h"

extern char ERROR_MESSAGE[] ;
extern bool VERBOSE ;

hash_table prune_vocabulary(hash_table vocab_hash, size_t min_count, size_t max_vocab, char* unk) {
  hashrec_type **hasharray, *htmp ;
  hash_value_type unknown_freq = hashprune(vocab_hash, min_count) ;
  size_t vocab_size = hashgetsize(vocab_hash) ;
  if( 0 < max_vocab && max_vocab < vocab_size) {
    hasharray = hashgetrecords(vocab_hash, HASH_DESCEND) ;
    unknown_freq += hashprune(vocab_hash, hasharray[max_vocab-1]->value) ;
    free(hasharray) ;
  } 

  htmp = hashsearch(vocab_hash, unk) ;
  if (htmp) htmp->value += unknown_freq ;
  else if (unknown_freq > 0) {
    htmp = hashinsert(vocab_hash, unk) ; htmp->value = unknown_freq ;
    if (VERBOSE) fprintf(stderr, "unknown is added to vocabulary (%s, %e)\n", unk, unknown_freq) ;
  }
  return vocab_hash ;
}

hashrec_type** load_vocabulary(const char *path, bool VERBOSE) {
  FILE *fid ;
  hashrec_type **vocab_hash ;

  if ( !(fid = my_fopen(path, "r")) ) return NULL ;

  if (VERBOSE) fprintf(stderr, "LOADING VOCABULARY ... ") ;
  vocab_hash = load_hash(fid) ;
  if (!vocab_hash) return NULL ;
  fclose(fid) ;
  if (VERBOSE) fprintf(stderr, "done [%zu unique words]\n", hashgetsize(vocab_hash));
  return vocab_hash ;
}

int raw_build_vocabulary(const char *path, hashrec_type ***vh, hashrec_type ***fh, bool normalization) {
  size_t i, n ;
  char *lineptr, *tok;
  long fsz ;
  FILE *fid ;
  hashrec_type *htmp, *snt_beg, *snt_end ;
  
  *fh = *vh = inithashtable() ;

  lineptr = tok = NULL ;
  htmp = snt_beg = snt_end = NULL ;
  i = 0 ;


  if ( !(fid = my_fopen(path, "r")) ) return -1 ;
  
  fsz = fsize(fid)  ;

  snt_beg = hashinsert(*vh, SENTENCE_BEG) ;
  snt_end = hashinsert(*vh, SENTENCE_END) ;

  if(VERBOSE) fprintf(stderr, "BUILDING VOCABULARY\n");
  if(VERBOSE) fprintf(stderr, "Processed %zu tokens.", i);
  while(getline(&lineptr, &n, fid) != -1) {
    snt_beg->value++ ;
    for (tok = strtok(lineptr, SPACE_DELIM) ; tok ; tok = strtok(NULL, SPACE_DELIM)) {
      if (normalization) tok = normalize_token(tok) ;
      if ( !(htmp = hashinsert(*vh, tok)) ) {
       sprintf(ERROR_MESSAGE, "cannot insert token \"%s\" into the vocabulary hash", tok) ;
       return -1 ;
      }
      htmp->value++ ;
      if (normalization) free(tok) ;
      if(((++i) % PROGRESS_BREAK) == 0) 
        if(VERBOSE) fprintf(stderr,"\033[11G%zu tokens [progress %1.2f%%]", i, (float)(ftell(fid)*100.0/fsz));
    }
    snt_end->value++ ;
  }
  free(lineptr) ;
  if (VERBOSE) fprintf(stderr, "\033[0GProcessed %zu tokens [progress %1.2f%%].\n", i, (float)(ftell(fid)*100.0/fsz));
  if (VERBOSE) fprintf(stderr, "done [%zu unique words has been counted]\n", hashgetsize(*vh));
  fclose(fid) ;
  return 0;
}

int annotated_build_vocabulary(const char *path, hashrec_type ***vh, hashrec_type ***fh, bool normalization) {
  size_t v, f, n, l;
  char *lineptr, *id, *word, *feat, *cntxt, *tok ;
  long fsz ;
  bool end_snt_set = false ;
  hashrec_type *htmp, *snt_beg_vocab, *snt_beg_feat, *snt_end_vocab, *snt_end_feat, *root_context_vocab, *root_context_feat ;
  FILE* fid ;

  v = f = l = 0 ;
  lineptr = id = word = feat = cntxt = tok = NULL ;
  htmp = snt_beg_vocab = snt_beg_feat = snt_end_vocab = snt_end_feat = root_context_vocab = root_context_feat = NULL ;

  *vh   = inithashtable() ;
  *fh = inithashtable() ;

  if ( !(fid = my_fopen(path, "r")) ) return -1 ;
  
  fsz = fsize(fid) ;

  snt_beg_feat  = hashinsert(*fh, SENTENCE_BEG)  ;
  snt_beg_vocab = hashinsert(*vh, SENTENCE_BEG) ;

  snt_end_feat  = hashinsert(*fh, SENTENCE_END)  ;
  snt_end_vocab = hashinsert(*vh, SENTENCE_END) ;

  root_context_feat   = hashinsert(*fh, ROOT_CONTEXT) ;
  root_context_vocab  = hashinsert(*vh, ROOT_CONTEXT) ;

  if(VERBOSE) fprintf(stderr, "BUILDING VOCABULARY and CONTEXTUAL FEATURE SET\n");
  if(VERBOSE) fprintf(stderr, "Processed %zu tokens, %zu features", v, f);

  snt_beg_feat->value++ ; 
  snt_beg_vocab->value++ ;

  while(getline(&lineptr, &n, fid) != -1) { 
    l++ ;
    if (end_snt_set == true) end_snt_set = false ;
    if ( !(id = strtok(lineptr, SPACE_DELIM)) ) {
      // empty line is seen
      snt_end_feat->value++ ;
      snt_end_vocab->value++ ;
      end_snt_set = true ;

      snt_beg_feat->value++ ;
      snt_beg_vocab->value++ ;

      continue ;
    }

    word  = strtok(NULL, SPACE_DELIM) ;
    cntxt = strtok(NULL, SPACE_DELIM) ;
    feat  = strtok(NULL, SPACE_DELIM) ;
    if (!word || !cntxt || !feat) {
      sprintf(ERROR_MESSAGE,"error at line %zu\n", l) ;
      return -2 ;
    }

    if (normalization) word = normalize_token(word) ;
    if ( !(htmp = hashinsert(*vh, word)) ) {
      sprintf(ERROR_MESSAGE, "cannot insert token \"%s\" into the vocabulary hash", word) ;
      return -1 ;
    }
      htmp->value++ ;
    if (normalization) free(word) ;
    v++ ;
    
    for(tok = strtok(cntxt, COMMA_DELIM) ; tok ; tok = strtok(NULL, COMMA_DELIM) )
      if (atol(tok) == 0) { 
        root_context_feat->value++ ;
        root_context_vocab->value++ ;
        break ;
      }

    for(tok = strtok(feat, COMMA_DELIM) ; tok ; tok = strtok(NULL, COMMA_DELIM) ) {
      if ( !(htmp = hashinsert(*fh, tok)) ) { 
        sprintf(ERROR_MESSAGE, "cannot insert token \"%s\" into the feature hash", tok) ;
        return -1 ;
      }
      htmp->value++ ;
      f++ ;
    }
    if((v % PROGRESS_BREAK) == 0) 
      if(VERBOSE) fprintf(stderr,"\033[11G%zu tokens, %zu features [progress %1.2f%%]", v, f, (float)(ftell(fid)*100.0/fsz));
  }
  if(VERBOSE) fprintf(stderr,"\033[11G%zu tokens, %zu features [progress %1.2f%%]\n", v, f, (float)(ftell(fid)*100.0/fsz));
  free(lineptr) ;
  fclose(fid) ;

  if (end_snt_set == false) {
    snt_end_feat->value++ ;
    snt_end_vocab->value++ ;
  }

  if(VERBOSE) fprintf(stderr, "Using vocabulary of size %zu and contextual feature set of size %zu.\n", hashgetsize(*vh), hashgetsize(*fh));
  return 1 ;
}

#endif
