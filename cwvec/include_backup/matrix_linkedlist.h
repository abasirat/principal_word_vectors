#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "basic_header.h"

#include <assert.h>
#include <math.h>


typedef struct column_list_item {
  matrix_index_type row ;
  matrix_element_type value ;
  struct column_list_item *next ;
} column_list_type ;

typedef struct {
  column_list_type **list ;
  matrix_element_type **array ;
} matrix_layer_type ;


typedef struct {
  matrix_index_type row ;
  matrix_index_type col ;
  matrix_index_type hgt ; 
  matrix_layer_type *layer ;
  size_t memory ;
  size_t *array_length ;
  long miss_hit ;
  long hit ;
} matrix_type ;



extern char ERROR_MESSAGE[] ;
extern size_t USED_MEM ;

void* my_malloc(size_t size) ;


void* my_calloc(size_t nmemb, size_t size) ;


//static size_t* set_array_length(matrix_type *matrix, double max_memory) ;
  
void matrix_delete(matrix_type* matrix) ;
 
matrix_type* matrix_init(matrix_index_type row, matrix_index_type col, matrix_index_type hgt, double max_memory, char* overflow_file) ;


matrix_element_type matrix_add_elem(matrix_type* matrix, matrix_index_type r, matrix_index_type c, matrix_index_type h, matrix_element_type v) ;


matrix_element_type matrix_get_elem(matrix_type* matrix, matrix_index_type r, matrix_index_type c, matrix_index_type h) ;

#endif
