#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "basic_header.h"

char    ERROR_MESSAGE[ERROR_SIZE] ;
bool    VERBOSE   = false ;
size_t  USED_MEM  = 0 ;

#include "hash.h"
#include "options.h"
#include "vocab_count.h"
#include "matrix.h"
#include "contextual_matrix.h"
#include "tools.h"


int build_vocabulary(const struct input_options_st *options, hash_table *vh, hash_table *fh) {
  int (*vocabulary_builder)(const char*, hash_table*, hash_table*, bool) = NULL ;

  if (options->corpus == RAW) vocabulary_builder = &raw_build_vocabulary ;
  else if (options->corpus == ANNOTATED) vocabulary_builder = &annotated_build_vocabulary ;

  if( 
      ( (options->corpus == RAW) && !options->load_vocabulary) 
      ||
      ( (options->corpus == ANNOTATED) && (!options->load_vocabulary || !options->load_features)) 
    )
      if ( ((*vocabulary_builder)(options->input_path, vh, fh, options->normalization)) < 0 ) 
        return -1 ;

  if (options->load_vocabulary) *vh = load_vocabulary(options->vocabulary_path) ;
  if (options->load_features) *fh = load_vocabulary(options->feature_path) ;

  if (options->corpus == RAW) *fh = hashduplicate(*vh) ;

  if (*vh) *vh = prune_vocabulary(*vh, options->min_vcount, options->max_vocab, UNKNOWN_WORD) ;
  if (*fh) *fh = prune_vocabulary(*fh, options->min_fcount, options->max_feature, UNKNOWN_FEAT) ;

  if (!*vh || !*fh) return -1 ;
  return 0 ;
}

contextual_matrix_type init_contetxual_matrix_from_options(const struct input_options_st *options) {
  contextual_matrix_type cnt_mat ;
  hashrec_type *htmp ;
  size_t r, c, h ;

  if ( build_vocabulary(options, &cnt_mat.vocab_hash, &cnt_mat.feature_hash) == -1 ) 
    error("get_global_data: ") ;
  if (!cnt_mat.vocab_hash)    error("vocab hash failed: ") ;
  if (!cnt_mat.feature_hash)  error("feature hash failed: ") ;
  save_hash(options->vocabulary_path, cnt_mat.vocab_hash, HASH_DESCEND) ;
  cnt_mat.vocab_hash = index_hash(cnt_mat.vocab_hash, HASH_DESCEND) ;
  
  save_hash(options->feature_path, cnt_mat.feature_hash, HASH_DESCEND) ;
  cnt_mat.feature_hash = index_hash(cnt_mat.feature_hash, HASH_DESCEND) ;

  cnt_mat.window_size         = options->window_size ;
  cnt_mat.symmetric           = options->symmetric ;
  if ( (htmp = hashsearch(cnt_mat.vocab_hash, SENTENCE_BEG)) != NULL )    cnt_mat.snt_beg_vocab = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.feature_hash, SENTENCE_BEG)) != NULL )  cnt_mat.snt_beg_feat  = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.vocab_hash, SENTENCE_END)) != NULL )    cnt_mat.snt_end_vocab = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.feature_hash, SENTENCE_END)) != NULL )  cnt_mat.snt_end_feat  = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.vocab_hash, UNKNOWN_WORD)) != NULL )    cnt_mat.unknown_vocab = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.feature_hash, UNKNOWN_FEAT)) != NULL )  cnt_mat.unknown_feat  = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.vocab_hash, ROOT_CONTEXT)) != NULL )    cnt_mat.root_context_vocab = htmp->value ;
  if ( (htmp = hashsearch(cnt_mat.feature_hash, ROOT_CONTEXT)) != NULL )  cnt_mat.root_context_feat = htmp->value ;

  c = hashgetsize(cnt_mat.vocab_hash) ;
  r = hashgetsize(cnt_mat.feature_hash) ;

  if (options->context == POW) h = (size_t)(abs(options->window_size)) ;
  else h = 1 ;
  if (options->window_size == 0) h = 1 ;
  if ( !(cnt_mat.matrix = matrix_init(r, c, h, options->max_memory, options->overflow_file)) ) 
    error("could not initialize matrix: ") ;

  return cnt_mat ;
}

int run(const struct input_options_st *options) {
  int (*matrix_filler)(char*, contextual_matrix_type*, bool, context_type) = NULL, status ;
  contextual_matrix_type cnt_mat ;

  VERBOSE = options->verbose ;
  
  if ( (access(options->input_path, R_OK) != -1) && (access(options->output_path, W_OK || F_OK) != -1) ) {
    sprintf(ERROR_MESSAGE, "access denied") ;
    error("trying to access input and output files: ") ;
    return -1 ;
  }

  cnt_mat = init_contetxual_matrix_from_options(options) ;

  if (options->corpus == RAW) matrix_filler = &raw_contextual_matrix ;
  else if (options->corpus == ANNOTATED) matrix_filler = &annotated_contextual_matrix ;
  if ( (status = (*matrix_filler)(options->input_path, &cnt_mat, options->normalization, options->context)) == -1 ) {
    error("could not fill the contetxual matrix: ") ;
    return -1 ;
  }

  if (matrix_save(options->output_path, cnt_mat.matrix, options->output_format) < 0) {
    error("could not save the contextual matrix: ") ;
    return -2 ;
  }

  if (options->print) matrix_print(cnt_mat.matrix, stdout) ;
  
  hashclear(&cnt_mat.vocab_hash) ;
  if (options->corpus == ANNOTATED) 
    hashclear(&cnt_mat.feature_hash) ;
  matrix_delete(cnt_mat.matrix) ;

  return 0 ;
}

int main(int argc, char* argv[]) {
  struct input_options_st options ;
  int status = parse_options(argc, argv, &options) ;
  if ( status < 0) {
    fprintf(stderr, "error in usage!\n") ;
    fprintf(stderr, "%s", ERROR_MESSAGE) ;
    return EXIT_FAILURE ;
  }
  else if (status == 1) return EXIT_FAILURE ;

  if (run(&options) < 0) {
    fprintf(stderr, "%s\n", ERROR_MESSAGE) ;
    return EXIT_FAILURE ;
  }
  delete_input_options(&options) ;
  return EXIT_SUCCESS ;
}

