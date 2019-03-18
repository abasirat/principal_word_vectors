#ifndef __CONTEXTUAL_MATRIX_H
#define __CONTEXTUAL_MATRIX_H

#include "hash.h"
#include "matrix.h"
#include "tools.h"
#include "str_norm.h"

extern bool VERBOSE ;
extern size_t  USED_MEM ;

typedef struct contextual_matrix_st {
  matrix_type *matrix ;
  hash_table vocab_hash ;
  hash_table feature_hash ;
  int window_size ;
  bool symmetric ;
  hash_value_type snt_beg_vocab ;
  hash_value_type snt_beg_feat ;
  hash_value_type snt_end_vocab ;
  hash_value_type snt_end_feat ;
  hash_value_type unknown_vocab ;
  hash_value_type unknown_feat ;
  hash_value_type root_context_vocab ;
  hash_value_type root_context_feat ;
} contextual_matrix_type ;

typedef struct word_struct {
  long id ;
  hash_value_type wid ;
  hash_value_type feature[MAX_FEATURE] ;
  long context[MAX_CONTEXT] ;
  size_t num_contexts ;
  size_t num_feats ;
} word_type ;



int raw_contextual_matrix(char *path, contextual_matrix_type* cnt_mat, bool normalization, context_type context) ;
int annotated_contextual_matrix(char *path, contextual_matrix_type* cnt_mat, bool normalization, context_type context) ;

#endif

