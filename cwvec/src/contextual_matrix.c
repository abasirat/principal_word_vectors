#include "contextual_matrix.h"

int neighbourhood_row_contextual_matrix(FILE* fid, contextual_matrix_type *cnt_mat, bool normalization) {
  char *lineptr, *tok, *saveptr ;
  long fsz ;
  bool is_last_token  ;
  size_t n, progress  ;
  int abs_win_sz, j ;
  matrix_index_type r, c, *window, itmp, widx ;
  hashrec_type *htmp ;

  lineptr = tok = saveptr = NULL ;
  fsz = fsize(fid) ;
  is_last_token = false ;
  progress = 0 ;
  r = c = 0 ;
  abs_win_sz = abs(cnt_mat->window_size) ;
  window = (matrix_index_type*)malloc((cnt_mat->window_size ? abs_win_sz : 1) * sizeof(matrix_index_type)) ;
  
  while(getline(&lineptr, &n, fid) != -1) {
    is_last_token = false ;
    j = ( (window[0] = cnt_mat->snt_beg_vocab) != -1 ) ? 1 : 0 ;

    for ( tok = strtok_r(lineptr, SPACE_DELIM, &saveptr) ; is_last_token == false ; tok = strtok_r(NULL, SPACE_DELIM, &saveptr)) {
      if (tok) { 
        if (normalization) tok = normalize_token(tok) ;
        htmp = hashsearch(cnt_mat->vocab_hash, tok) ;
        if (normalization) free(tok) ;
        itmp = (htmp) ? (matrix_index_type)(htmp->value) : cnt_mat->unknown_vocab ;
      }
      else {
        is_last_token = true ; 
        if ( (itmp = cnt_mat->snt_end_vocab) == -1 ) continue ;
      }

      widx = itmp ;
      if (cnt_mat->window_size < 0) c = itmp; else r = itmp ;
      if (j >= abs_win_sz) {
        itmp = window[j % abs_win_sz] ;
        if (cnt_mat->window_size < 0) r = itmp; else c = itmp ;
        if ( matrix_add_elem(cnt_mat->matrix, r, c, 0, 1.0) < 0 ) return -1 ;
        if ( cnt_mat->symmetric == true && matrix_add_elem(cnt_mat->matrix, c, r, 0, 1.0) < 0 ) return -1 ;
      }
    
      if(((++progress) % PROGRESS_BREAK) == 0) 
#ifdef _USE_LINKED_LIST_
        if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]", 
            progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
            USED_MEM*1.0/OneGigabyte);
#else
        if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f, used_buffer=%2.2f]", 
            progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
            USED_MEM*1.0/OneGigabyte, cnt_mat->matrix->layer[0].ind*100.0/cnt_mat->matrix->layer[0].buffer_size);
#endif


      window[j % abs_win_sz] = widx ;
      j++ ;
    }
  }
#ifdef _USE_LINKED_LIST_
  if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]\n", 
            progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
            USED_MEM*1.0/OneGigabyte);
#else
  if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f, used_buffer=%2.2f]\n", 
      progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
      USED_MEM*1.0/OneGigabyte, cnt_mat->matrix->layer[0].ind*100.0/cnt_mat->matrix->layer[0].buffer_size);
#endif

  free(window) ;


  return 0 ;
}

int bow_row_contextual_matrix(FILE* fid, contextual_matrix_type *cnt_mat, bool normalization) {
  char *lineptr, *tok, *saveptr ;
  long fsz ;
  bool is_last_token  ;
  size_t n, progress  ;
  int abs_win_sz, k, j ;
  matrix_index_type r, c, *window, itmp, widx ;
  hashrec_type *htmp ;

  lineptr       = tok = saveptr = NULL ;
  fsz           = fsize(fid) ;
  is_last_token = false ;
  progress      = 0 ;
  r             = c = widx = 0 ;
  abs_win_sz    = abs(cnt_mat->window_size) ;
  window        = (matrix_index_type*)malloc((cnt_mat->window_size ? abs_win_sz : 1) * sizeof(matrix_index_type)) ;
  
  while(getline(&lineptr, &n, fid) != -1) {
    is_last_token = false ;
    j = ( (window[0] = cnt_mat->snt_beg_vocab) != -1 ) ? 1 : 0 ;

    for ( tok = strtok_r(lineptr, SPACE_DELIM, &saveptr) ; is_last_token == false ; tok = strtok_r(NULL, SPACE_DELIM, &saveptr)) {
       if (tok) { 
        if (normalization) tok = normalize_token(tok) ;
        htmp = hashsearch(cnt_mat->vocab_hash, tok) ;
        if (normalization) free(tok) ;
        itmp = (htmp) ? (matrix_index_type)(htmp->value) : cnt_mat->unknown_vocab ;
      }
      else {
        is_last_token = true ; 
        if ( (itmp = cnt_mat->snt_end_vocab) == -1 ) continue ;
      }

      widx = itmp ;
      if (cnt_mat->window_size < 0) c = itmp; else r = itmp ;
      for ( k=j-1 ; k >= ( (j > abs_win_sz) ? j-abs_win_sz : 0 ); k--) {
        itmp = window[k % abs_win_sz] ;
        if (cnt_mat->window_size < 0) r = itmp; else c = itmp ;
        if ( matrix_add_elem(cnt_mat->matrix, r, c, 0, 1.0/(j-k)) < 0 ) return -1 ;
        if ( cnt_mat->symmetric == true && matrix_add_elem(cnt_mat->matrix, c, r, 0, 1.0/(j-k) ) < 0) return -1 ;
      }
    
      if(((++progress) % PROGRESS_BREAK) == 0) 
        if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]", 
            progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
            USED_MEM*1.0/OneGigabyte);

      window[j % abs_win_sz] = widx ;
      j++ ;
    }
  }
  if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]\n", 
      progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
      USED_MEM*1.0/OneGigabyte);

  free(window) ;
  return 0 ;
}

int pow_row_contextual_matrix(FILE* fid, contextual_matrix_type *cnt_mat, bool normalization) {
  char *lineptr, *tok, *saveptr ;
  long fsz ;
  bool is_last_token  ;
  size_t n, progress  ;
  int abs_win_sz, k, j ;
  matrix_index_type r, c, *window, itmp, widx ;
  hashrec_type *htmp ;

  lineptr = tok = saveptr = NULL ;
  fsz = fsize(fid) ;
  is_last_token = false ;
  progress = 0 ;
  r = c = widx = 0 ;
  abs_win_sz = abs(cnt_mat->window_size) ;
  window = (matrix_index_type*)malloc((cnt_mat->window_size ? abs_win_sz : 1) * sizeof(matrix_index_type)) ;
  
  while(getline(&lineptr, &n, fid) != -1) {
    is_last_token = false ;
    j = ( (window[0] = cnt_mat->snt_beg_vocab) != -1 ) ? 1 : 0 ;

    for ( tok = strtok_r(lineptr, SPACE_DELIM, &saveptr) ; is_last_token == false ; tok = strtok_r(NULL, SPACE_DELIM, &saveptr)) {
      if (tok) { 
        if (normalization) tok = normalize_token(tok) ;
        htmp = hashsearch(cnt_mat->vocab_hash, tok) ;
        if (normalization) free(tok) ;
        itmp = (htmp) ? (matrix_index_type)(htmp->value) : cnt_mat->unknown_vocab ;
      }
      else {
        is_last_token = true ; 
        if ( (itmp = cnt_mat->snt_end_vocab) == -1 ) continue ;
      }

      widx = itmp ;
      if (cnt_mat->window_size < 0) c = itmp; else r = itmp ;
      for ( k=j-1 ; k >= ( (j > abs_win_sz) ? j-abs_win_sz : 0 ); k--) {
        itmp = window[k % abs_win_sz] ;
        if (cnt_mat->window_size < 0) r = itmp; else c = itmp ;
        if ( matrix_add_elem(cnt_mat->matrix, r, c, j-k-1, 1.0) < 0 ) return -1 ;
        if ( cnt_mat->symmetric == true && matrix_add_elem(cnt_mat->matrix, c, r, j-k-1, 1.0) < 0) return -1 ;
      }
    
      if(((++progress) % PROGRESS_BREAK) == 0) 
        if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]", 
            progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
            USED_MEM*1.0/OneGigabyte);
      window[j % abs_win_sz] = widx ;
      j++ ;
    }
  }
  if(VERBOSE) fprintf(stderr, "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]\n", 
      progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit),
      USED_MEM*1.0/OneGigabyte);

  free(window) ;
  return 0 ;
}

//int raw_contextual_matrix(char *path, matrix_type* matrix, hash_table vocab_hash, hash_table feature_hash, int window_size, bool symmetric, context_type context, bool normalization) {
int raw_contextual_matrix(char *path, contextual_matrix_type* cnt_mat, bool normalization, context_type context) {
  FILE *fid ;
  //hashrec_type* htmp ;
  //hash_value_type snt_beg = -1, snt_end = -1, unk = -1 ;
  //contextual_matrix_type cnt_mat ;
  int status = -1;

  if ( (fid = my_fopen(path, "r")) == NULL ) return -1 ;

  /*if ((htmp = hashsearch(vocab_hash, UNKNOWN_WORD))) unk    = (matrix_index_type)htmp->value ;
  if ((htmp = hashsearch(vocab_hash, SENTENCE_BEG))) snt_beg = (matrix_index_type)htmp->value ;
  if ((htmp = hashsearch(vocab_hash, SENTENCE_END))) snt_end = (matrix_index_type)htmp->value ;

  cnt_mat.matrix              = matrix ;
  cnt_mat.vocab_hash          = vocab_hash ;
  cnt_mat.feature_hash        = feature_hash ;
  cnt_mat.window_size         = window_size ;
  cnt_mat.symmetric           = symmetric ;
  cnt_mat.snt_beg_vocab       = snt_beg ;
  cnt_mat.snt_beg_feat        = snt_beg ;
  cnt_mat.snt_end_vocab       = snt_end ;
  cnt_mat.snt_end_feat        = snt_end ;
  cnt_mat.unknown_vocab       = unk ;
  cnt_mat.root_context_vocab  = -1 ;
  cnt_mat.root_context_feat   = -1;*/

  if ( context == NEIGHBOURHOOD)
    status = neighbourhood_row_contextual_matrix(fid, cnt_mat, normalization) ;
  else if ( context == BOW)
    status = bow_row_contextual_matrix(fid, cnt_mat, normalization) ;
  else if ( context == POW)
    status = pow_row_contextual_matrix(fid, cnt_mat, normalization) ;

  fclose(fid) ;
  return status ;
}

//////////////////////////////////////////
//
// annotated corpus
//
// //////////////////////////////////////

int parse_line(char *lineptr, word_type *word, contextual_matrix_type *cnt_mat, bool normalization) {
  char *id, *wid, *feat, *cntxt, *tok ;
  hashrec_type* htmp ;
  size_t i = 0 ;

  /*static hash_value_type unk = -1 ;
  if (unk == -1) 
    if ( ( htmp = hashsearch(vh, UNKNOWN_WORD) ) ) 
      unk = (matrix_index_type)htmp->value ;*/

  if ( !(id = strtok(lineptr, SPACE_DELIM)) ) return 0 ;
  wid   = strtok(NULL, SPACE_DELIM) ;
  cntxt = strtok(NULL, SPACE_DELIM) ;
  feat  = strtok(NULL, SPACE_DELIM) ;

  word->id = atol(id) ;

  if (normalization) wid = normalize_token(wid) ;
  htmp = hashsearch(cnt_mat->vocab_hash, wid) ;
  if (normalization) free(wid) ;
  word->wid = (htmp) ? htmp->value : cnt_mat->unknown_vocab ;
  
  for(i = 0, tok = strtok(feat, COMMA_DELIM) ;
      tok && i < MAX_FEATURE-1 ; // one feature is reserved for NO_CONTEXT
      i++ , tok = strtok(NULL, COMMA_DELIM) ) {
    htmp = hashsearch(cnt_mat->feature_hash, tok) ;
    word->feature[i] = (htmp) ? htmp->value : cnt_mat->unknown_feat ;
  }
  word->num_feats = i ;
       
  for(i = 0, tok = strtok(cntxt, COMMA_DELIM) ;
      tok && i < MAX_CONTEXT ; 
      i++ , tok = strtok(NULL, COMMA_DELIM) ) 
    word->context[i] = atol(tok) ;
  word->num_contexts = i ;
 
  return 1 ;
}

int update_contextual_matrix(contextual_matrix_type *cnt_mat, word_type* sentence, size_t sentence_length, context_type context) {
  int i, j, k, f, w, c ;
  int win_sz = cnt_mat->window_size ;
  int abs_win_sz = abs(cnt_mat->window_size) ;
  matrix_type *matrix = cnt_mat->matrix ;

  for (i = 0 ; i < sentence_length ; i++) {
    switch (context) {
      case NEIGHBOURHOOD :
        j = i + win_sz ;
        if ( j < 0 || j >= sentence_length ) continue ;
        for ( f = 0 ; f < sentence[j].num_feats ; f++ ) 
          if ( matrix_add_elem(matrix, sentence[j].feature[f], sentence[i].wid, 0, 1.0) < 0 ) 
            return -1 ;
        if ( cnt_mat->symmetric == true )
          for ( f = 0 ; f < sentence[i].num_feats ; f++ ) 
            if ( matrix_add_elem(matrix, sentence[i].feature[f], sentence[j].wid, 0, 1.0) < 0 ) 
              return -1 ;
        break ;
      case BOW :
        for ( j = i - 1 ; j >= ( (i > abs_win_sz) ? i - abs_win_sz : 0 ) ; j-- ) {
          if (win_sz > 0) { w = j ; c = i ; } else { w = i ; c = j ; } 
          for ( f = 0 ; f < sentence[c].num_feats ; f++ ) 
            if ( matrix_add_elem(matrix, sentence[c].feature[f], sentence[w].wid, 0, 1.0/(i-j) ) < 0 ) return -1 ;
          if ( cnt_mat->symmetric == true )
            for ( f = 0 ; f < sentence[w].num_feats ; f++ ) 
              if ( matrix_add_elem(matrix, sentence[w].feature[f], sentence[c].wid, 0, 1.0/(i-j)) < 0 ) return -1 ;
        }
        break ;
      case POW :
        for ( j = i - 1 ; j >= ( (i > abs_win_sz) ? i - abs_win_sz : 0 ) ; j-- ) {
          if (win_sz > 0) { w = j ; c = i ; } else { w = i ; c = j ; } 
          for ( f = 0 ; f < sentence[c].num_feats ; f++ ) 
            if ( matrix_add_elem(matrix, sentence[c].feature[f], sentence[w].wid, i-j-1, 1.0) < 0 ) return -1 ;
          if ( cnt_mat->symmetric == true )
            for ( f = 0 ; f < sentence[w].num_feats ; f++ ) 
              if ( matrix_add_elem(matrix, sentence[w].feature[f], sentence[c].wid, i-j-1, 1.0) < 0 ) return -1 ;
        }
        break ;
      case INDEXED :
        for ( k = 0 ; k < sentence[i].num_contexts ; k++ ) {
          if ( (j = sentence[i].context[k]) >= MAX_SENTENCE_LENGTH ) continue ;
          for ( f = 0 ; f < sentence[j].num_feats ; f++ ) 
            if ( matrix_add_elem(matrix, sentence[j].feature[f], sentence[i].wid, 0, 1.0) < 0 ) return -1 ;
          if ( cnt_mat->symmetric == true )
            for ( f = 0 ; f < sentence[i].num_feats ; f++ ) 
              if ( matrix_add_elem(matrix, sentence[i].feature[f], sentence[j].wid, 0, 1.0) < 0 ) return -1 ;
        }
        break ;
    }
  }
  return 0 ;
}

//int annotated_contextual_matrix(char *path, matrix_type* matrix, hash_table vocab_hash, hash_table feature_hash, int window_size, bool symmetric, context_type context, bool normalization) {
int annotated_contextual_matrix(char *path, contextual_matrix_type* cnt_mat, bool normalization, context_type context) {
  FILE *fid ;
  char *lineptr = NULL ;
  long fsz ;
  size_t n, progress = 0, i = 0;
  //hashrec_type *htmp ;
  word_type sentence[MAX_SENTENCE_LENGTH], sbeg, send ;
  //contextual_matrix_type cnt_mat ;
  //hash_value_type vbeg, vend, fbeg, fend, frcntxt, vrcntxt, unk ;

  if ( !(fid = my_fopen(path, "r")) ) return -1 ;
  fsz = fsize(fid) ;

  /*vbeg = vend = fbeg = fend = frcntxt = vrcntxt = unk = -1 ;

  if ( ( htmp = hashsearch(vocab_hash, UNKNOWN_WORD) ) )    unk     = (matrix_index_type)htmp->value ;
  if ( ( htmp = hashsearch(vocab_hash, SENTENCE_BEG) ) )    vbeg    = (matrix_index_type)htmp->value ;
  if ( ( htmp = hashsearch(feature_hash, SENTENCE_BEG) ) )  fbeg    = (matrix_index_type)htmp->value ;
  if ( ( htmp = hashsearch(vocab_hash, SENTENCE_END) ) )    vend    = (matrix_index_type)htmp->value ;
  if ( ( htmp = hashsearch(feature_hash, SENTENCE_END) ) )  fend    = (matrix_index_type)htmp->value ;
  if ( ( htmp = hashsearch(vocab_hash, ROOT_CONTEXT) ) )    vrcntxt = (matrix_index_type)htmp->value ;
  if ( ( htmp = hashsearch(feature_hash, ROOT_CONTEXT) ) )  frcntxt = (matrix_index_type)htmp->value ;

  cnt_mat.matrix              = matrix ;
  cnt_mat.vocab_hash          = vocab_hash ;
  cnt_mat.feature_hash        = feature_hash ;
  cnt_mat.window_size         = window_size ;
  cnt_mat.symmetric           = symmetric ;
  cnt_mat.snt_beg_vocab       = vbeg ;
  cnt_mat.snt_beg_feat        = fbeg ;
  cnt_mat.snt_end_vocab       = vend ;
  cnt_mat.snt_end_feat        = fend ;
  cnt_mat.unknown_vocab       = unk ;
  cnt_mat.root_context_vocab  = vrcntxt ;
  cnt_mat.root_context_feat   = frcntxt;*/

  sbeg.id             = 0 ; 
  if (context == INDEXED) {
    sbeg.wid          = cnt_mat->root_context_vocab ; 
    sbeg.feature[0]   = cnt_mat->root_context_feat ; 
  }
  else {
    sbeg.wid          = cnt_mat->snt_beg_vocab ; 
    sbeg.feature[0]   = cnt_mat->snt_beg_feat ; 
  }
  sbeg.num_feats      = 1 ; 
  sbeg.num_contexts   = 0 ;

  send.id             = -1; 
  send.wid            = cnt_mat->snt_end_vocab ; 
  send.feature[0]     = cnt_mat->snt_end_feat ; 
  send.num_feats      = 1 ; 
  send.num_contexts   = 0 ;

  sentence[0] = sbeg ;
  i = 0 ;
  while( getline(&lineptr, &n, fid) != -1 ) {
    if (++i >= MAX_SENTENCE_LENGTH-1  || !parse_line(lineptr, sentence + i, cnt_mat, normalization)) { 
      send.id = i ;
      sentence[i] = send ;
      if (update_contextual_matrix(cnt_mat, sentence, i + 1, context) < 0 ) return -1 ;
      i = 0 ;
      continue ;
    }
    //i++ ;
    if(VERBOSE) if(((++progress)%PROGRESS_BREAK) == 0) fprintf(stderr,  
        "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]", 
        progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit), 
        USED_MEM*1.0/OneGigabyte);
  } 
  
  if ( i ) {
    i++ ;
    send.id = i ;
    sentence[i] = send ;
    if (update_contextual_matrix(cnt_mat, sentence, i + 1, context) < 0 ) return -1 ;
  }

  if(VERBOSE) fprintf(stderr, 
      "\033[0G%zu items. [progress: %1.2f%%, missed hit ratio: %1.2f%%, used_mem=%2.2f]\n", 
      progress, (float)(ftell(fid)*100.0/fsz), cnt_mat->matrix->miss_hit*100.0/(cnt_mat->matrix->miss_hit + cnt_mat->matrix->hit), 
      USED_MEM*1.0/OneGigabyte);

  free(lineptr) ;
  fclose(fid) ;
  return 0 ;
}

