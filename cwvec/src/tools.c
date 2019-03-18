#ifndef __TOOLS_C
#define __TOOLS_C

#include "tools.h"

extern char ERROR_MESSAGE[] ;

#ifdef _USE_LINKED_LIST_
int matrix_save(char* path, matrix_type* matrix, output_format_type output_format) {
  matrix_index_type r, c, h;
  matrix_element_type elem ;
  column_list_type *ctmp ;
  char *format = (char*)"%zu\t%zu\t%zu\t%e\n" ;
  FILE *fid ;

  if ( (fid = fopen(path, "w")) == NULL) {
    sprintf(ERROR_MESSAGE, "matrix_save: cannot open file to write on") ;
    return -1 ;
  }

  //fprintf(stderr,"%d\n", output_format) ;

  /*if (output_format == BinaryOutMode) {
    fwrite(&matrix->row, sizeof(matrix_index_type), 1, fid) ;
    fwrite(&matrix->col, sizeof(matrix_index_type), 1, fid) ;
  }*/

  for (h = 0 ; h < matrix->hgt ; h++) {
    for (c = 0 ; c < matrix->col ; c++) {
      for (r = 0 ; r < matrix->array_length[c] ; r++) {
        elem = *(matrix->layer[h].array[c] + r) ;
        if (elem > 0) {
          if (output_format == BIN) {
            fwrite(&r, sizeof(matrix_index_type), 1, fid) ;
            fwrite(&c, sizeof(matrix_index_type), 1, fid) ;
            fwrite(&h, sizeof(matrix_index_type), 1, fid) ;
            fwrite(&elem, sizeof(matrix_element_type), 1, fid) ;
          }
          else if (output_format == TXT) 
            fprintf(fid, format, r, c, h, elem) ;
        }
      }
      for (ctmp = matrix->layer[h].list[c] ; ctmp ; ctmp = ctmp->next) {
        if (output_format == BIN) {
          fwrite(&ctmp->row, sizeof(matrix_index_type), 1, fid) ;
          fwrite(&c, sizeof(matrix_index_type), 1, fid) ;
          fwrite(&h, sizeof(matrix_index_type), 1, fid) ;
          fwrite(&ctmp->value, sizeof(matrix_element_type), 1, fid) ;
        }
        else if (output_format == TXT) 
          fprintf(fid, format, ctmp->row, c, h, ctmp->value) ;
      }
    }
  }

  // write the last elemet in the matrix even if it is zero
  // The dimensions of the matrix then can be computed by gettining maximum value of rows and columns
  r = matrix->row - 1 ;
  c = matrix->col - 1 ;
  h = matrix->hgt - 1 ;
  elem = matrix_get_elem(matrix, r, c, h) ;
  if (elem == 0) { // if it is not zero then it has been written by the lines above 
    if (output_format == BIN) {
      fwrite(&r, sizeof(matrix_index_type), 1, fid) ;
      fwrite(&c, sizeof(matrix_index_type), 1, fid) ;
      fwrite(&h, sizeof(matrix_index_type), 1, fid) ;
      fwrite(&elem, sizeof(matrix_element_type), 1, fid) ;
    }
    else if (output_format == TXT) 
      fprintf(fid, format, r, c, h, elem) ;
  }
  return 0 ;
}
#endif

void matrix_print(matrix_type* matrix, FILE* fid) {
  matrix_index_type r, c, h ;
  for (h = 0 ; h < matrix->hgt ; h++) {
    fprintf(fid, "Begin of layer %zu\n",h) ;
    for (r = 0 ; r < matrix->row ; r++) {
      for (c = 0 ; c < matrix->col ; c++ )
       fprintf(fid,"\t%e", matrix_get_elem(matrix, r, c, h)) ;
      fprintf(fid, "\n") ;
    }
    fprintf(fid, "end of layer %zu\n",h) ;
  }
}

#define MIN(X,Y) (X < Y) ? X : Y
#ifndef ERROR_SIZE
#define ERROR_SIZE 128
#endif
FILE* my_fopen(const char* path, const char* mode) {
  FILE *fid ;
  if ( !(fid = fopen(path, mode)) ) {
      strcpy(ERROR_MESSAGE, "cannot open file ") ;
      strncat(ERROR_MESSAGE, path, MIN(ERROR_SIZE - strlen(ERROR_MESSAGE) - 1, strlen(path))) ;
  }
  return fid ;
}

long fsize(FILE* fid) {
  long cpos, size ;
  cpos = ftell(fid) ;
  fseek(fid, 0, SEEK_END) ;
  size = ftell(fid) ;
  fseek(fid, cpos, SEEK_SET) ;
  return size ;
}

hashrec_type** load_hash(FILE* fid) {
  hashrec_type *htmp = NULL, **hash = NULL ;
  char *lineptr = NULL, *nptr, *endptr ;
  size_t n, line_number = 0 ;
  hash_value_type value ;

  hash = inithashtable() ;
  while(getline(&lineptr, &n, fid) != -1) {
    ++line_number ;
    nptr = strchr(lineptr, '\t') ;
    *nptr = '\0' ;
    value = (hash_value_type)(strtold(++nptr, &endptr)) ;
    if ((value == 0) && (endptr == nptr)) {
      fprintf(stderr, "invalid hash entry [line %zu]\n", line_number + 1) ;
      return NULL ;
    }
    htmp = hashinsert(hash, lineptr) ;
    htmp->value = value ;
  }
  free(lineptr) ;
  return hash ;
}

int save_hash(const char* path, hash_table ht, hash_sort_type adescend) {
  FILE *fid ;
  size_t hsize, i ;
  hashrec_type** hasharray ;

  if ( !(fid = fopen(path, "w")) ) {
    fprintf(stderr, "cannot create hash file %s\n", path) ;
    return -1 ;
  }

  hasharray = hashgetrecords(ht, adescend) ;
  hsize = hashgetsize(ht) ;
  for(i = 0; i < hsize; i++) fprintf(fid, "%s\t%e\n", hasharray[i]->key, hasharray[i]->value) ;
  free(hasharray) ;
  fclose(fid) ;
  return 0 ;
}


hashrec_type** index_hash(hash_table ht, hash_sort_type adescend) {
  size_t hsize, i ;
  hash_value_type index = 0 ;
  hashrec_type **hasharray ;

  hasharray = hashgetrecords(ht, adescend) ;
  hsize = hashgetsize(ht) ;
  for(i = 0; i < hsize; i++) hasharray[i]->value = index++ ;
  free(hasharray) ;

  return ht ;
}

void error(char* msg) {
  fprintf(stderr,"%s %s\n", msg, ERROR_MESSAGE) ;
  exit(EXIT_FAILURE) ;
}

#endif
