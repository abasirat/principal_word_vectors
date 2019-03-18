/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 **/

#ifndef __MATRIX_C__
#define __MATRIX_C__

#include "matrix.h"

void* my_malloc(size_t size) {
  USED_MEM += size ;
  return malloc(size) ;
}

void* my_calloc(size_t nmemb, size_t size) {
  USED_MEM += size*nmemb ;
  return calloc(nmemb, size) ;
}

static size_t* set_array_length(matrix_type *matrix, double max_memory) {
  double nelem ;
  size_t *array, x = 0 ;
  matrix_index_type a, r, c, h ;
  
  r = matrix->row ;
  c = matrix->col ;
  h = matrix->hgt ;

  if ( (array = (size_t*)my_calloc(c, sizeof(size_t))) == NULL ) return NULL ;

  // nelem is the number of elements that can be kept in each layer of the dense memory
  // 0.1 is reserved for column lists
  nelem = (1 - 0.1) * max_memory/h/sizeof(matrix_element_type) ;
  
  // increase x, the number of columns with r items, until you reach nelem
  // sum of harmonic series n = r * x + r * ( 1 + 1/2 + ... + 1/(c-x) ) > r * x + r * log(c - x + 1))
  while ( ( r * (x + log(c - x + 1) ) < nelem )  && (x < c) )  x++ ;

  for (a = 0 ; a < c && nelem > 0 ; a++){ 
    if (a < x) array[a] = r ;
    else array[a] = r / (a - x + 1) ;
    nelem -= array[a];
  }
  return array ;
}

void matrix_delete(matrix_type* matrix) {
  matrix_index_type c, h ;
  column_list_type *ctmp, *cprv ;
  if (matrix == NULL) return ;
  for (h = 0 ; h < matrix->hgt ; h++) {
    if (matrix->layer[h].array) {
      for (c = 0 ; c < matrix->col ; c++) 
        if (matrix->layer[h].array[c]) free(matrix->layer[h].array[c]) ;
      free(matrix->layer[h].array) ;
    }

    if (matrix->layer[h].list) {
      for (c = 0 ; c < matrix->col ; c++) 
        for (ctmp = matrix->layer[h].list[c], cprv = NULL ; ctmp != NULL ; cprv=ctmp, ctmp = ctmp->next) 
          if (cprv) free(cprv) ;
      free(matrix->layer[h].list) ;
    }
  }
  if (matrix->array_length) free(matrix->array_length) ;
  if (matrix->layer) free(matrix->layer) ;
  free(matrix) ;
}

matrix_type* matrix_init(matrix_index_type row, matrix_index_type col, matrix_index_type hgt, double max_memory, char* overflow_file) {
  int h ;
  matrix_index_type a ;

  assert(row >= 0) ;
  assert(col >= 0) ;
  assert(hgt >= 1) ;
  assert(max_memory >= 0) ;

  matrix_type *matrix = (matrix_type*)my_malloc(sizeof(matrix_type)) ;
  matrix->row = row ;
  matrix->col = col ;
  matrix->hgt = hgt ;
  matrix->miss_hit = 0 ;
  matrix->hit = 0 ;

  matrix->array_length = set_array_length(matrix, max_memory) ;
  if (matrix->array_length == NULL) {
    sprintf(ERROR_MESSAGE, "init_matrix: memory allocation failed!") ;
    matrix_delete(matrix) ; 
    return NULL ;
  }

  matrix->layer = (matrix_layer_type*)my_malloc(hgt * sizeof(matrix_layer_type)) ;
  for (h = 0 ; h < hgt ; h++) {
    matrix->layer[h].array = (matrix_element_type**)my_malloc(col * sizeof(matrix_element_type*)) ;
    if (matrix->layer[h].array == NULL) {
      sprintf(ERROR_MESSAGE, "init_matrix: dense memory allocation failed!");
      matrix_delete(matrix) ;
      return NULL ;
    }
    for (a = 0 ; a < col ; a++) {
      matrix->layer[h].array[a] = (matrix_element_type*)my_calloc(matrix->array_length[a], sizeof(matrix_element_type)) ;
      if (matrix->layer[h].array[a] == NULL) matrix->array_length[a] = 0 ; // could not allocate memory
    }

    matrix->layer[h].list = (column_list_type**)calloc(col, sizeof(column_list_type*)) ;
    if (matrix->layer[h].list == NULL) {
      sprintf(ERROR_MESSAGE ,"init_matrix: sparse memory allocation failed!") ;
      matrix_delete(matrix) ;
      return NULL ;
    }
  }
  return matrix ;
}

matrix_element_type matrix_add_elem(matrix_type* matrix, matrix_index_type r, matrix_index_type c, matrix_index_type h, matrix_element_type v) {
  column_list_type *cprv, *ctmp ;

  //fprintf(stderr,"(%zu,%zu,%zu) (%zu,%zu,%zu) \n",r,c,h,matrix->row,matrix->col,matrix->hgt) ;
  assert((r < matrix->row) && (r >= 0)) ;
  assert((c < matrix->col) && (c >= 0)) ;
  assert((h < matrix->hgt) && (h >= 0)) ;

  if (r < matrix->array_length[c]) {
    matrix->hit++ ;
    return (*(matrix->layer[h].array[c] + r) += v) ;
  }
  else {
    return 0 ;
   for (cprv = NULL, ctmp = matrix->layer[h].list[c] ; ctmp && (ctmp->row != r); cprv = ctmp, ctmp = ctmp->next) ;
   if (ctmp == NULL) {
    if ((ctmp = (column_list_type*)my_malloc(sizeof(column_list_type))) == NULL) {
      sprintf(ERROR_MESSAGE, "matrix_elem_add: memory allocation failed!") ;
      return -1 ;
    }
    ctmp->row = r ;
    ctmp->value = v ;
    ctmp->next = NULL ;
    if (cprv == NULL) matrix->layer[h].list[c] = ctmp; 
    else cprv->next = ctmp ;
   }
   else { 
     ctmp->value += v; 
     if (cprv != NULL) {
       cprv->next = ctmp->next ;
       ctmp->next = matrix->layer[h].list[c] ;
       matrix->layer[h].list[c] = ctmp ;
     }
   }

   matrix->miss_hit++ ;
  }
  return ctmp->value ;
}

matrix_element_type matrix_get_elem(matrix_type* matrix, matrix_index_type r, matrix_index_type c, matrix_index_type h) {
  column_list_type *ctmp = NULL ;

  assert((r < matrix->row) && (r >= 0)) ;
  assert((c < matrix->col) && (c >= 0)) ;
  assert((h < matrix->hgt) && (h >= 0)) ;

  if (r < matrix->array_length[c])
    return *(matrix->layer[h].array[c] + r) ;
  else 
    for (ctmp = matrix->layer[h].list[c] ; ctmp && (ctmp->row != r); ctmp = ctmp->next) ;
  return ctmp ? ctmp->value : 0.0 ;
}
#endif
