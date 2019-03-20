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
#ifdef _USE_LINKED_LIST
  column_list_type *ctmp, *cprv ;
#endif
  if (matrix == NULL) return ;
  for (h = 0 ; h < matrix->hgt ; h++) {
    if (matrix->layer[h].array) {
      for (c = 0 ; c < matrix->col ; c++) 
        if (matrix->layer[h].array[c]) free(matrix->layer[h].array[c]) ;
      free(matrix->layer[h].array) ;
    }

#ifdef _USE_LINKED_LIST
    if (matrix->layer[h].list) {
      for (c = 0 ; c < matrix->col ; c++) 
        for (ctmp = matrix->layer[h].list[c], cprv = NULL ; ctmp != NULL ; cprv=ctmp, ctmp = ctmp->next) 
          if (cprv) free(cprv) ;
      free(matrix->layer[h].list) ;
    }
#endif
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
#ifndef _USE_LINKED_LIST_
  matrix->overflow_file = overflow_file ;
#endif


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

#ifdef _USE_LINKED_LIST_
    matrix->layer[h].list = (column_list_type**)calloc(col, sizeof(column_list_type*)) ;
    if (matrix->layer[h].list == NULL) {
      sprintf(ERROR_MESSAGE ,"init_matrix: sparse memory allocation failed!") ;
      matrix_delete(matrix) ;
      return NULL ;
    }
#else
    matrix->layer[h].buffer_size = 0.1 * max_memory/hgt/sizeof(buffer_item_type) ;
    matrix->layer[h].overflow_file_counter = 0 ;
    matrix->layer[h].ind = 0 ;
    matrix->layer[h].buffer = (buffer_item_type*)my_malloc(matrix->layer[h].buffer_size * sizeof(buffer_item_type)) ;
    if ( matrix->layer[h].buffer == NULL ) {
      sprintf(ERROR_MESSAGE ,"init_matrix: buffer allocation failed!") ;
      matrix_delete(matrix) ;
      return NULL ;
    }
#endif
  }
  return matrix ;
}


#ifndef _USE_LINKED_LIST_
int compare_matrix_elem(const void * a, const void * b) {
  int c ;
  if ( ( c = ((buffer_item_type*)(a))->hgt - ((buffer_item_type*)(b))->hgt ) != 0 ) return c ;
  if ( ( c = ((buffer_item_type*)(a))->col - ((buffer_item_type*)(b))->col ) != 0 ) return c ;
  return ( ((buffer_item_type*)(a))->row - ((buffer_item_type*)(b))->row ) ;
}

/* Check if two cooccurrence records are for the same two words */
int compare_buffer_item_id(buffer_item_id_type a, buffer_item_id_type b) {
    int c;
    if( (c = a.hgt - b.hgt) != 0) return c ;
    if( (c = a.col - b.col) != 0) return c ;
    else return a.row - b.row ;
}


int write_buffer(buffer_item_type* buffer, size_t size, size_t nmemb, const char* file_name, float rate) {
  FILE* fid ;
  size_t i, j ;
  // This function does not work correctly for rate > 0, hence it is called with rate = 0
  // The idea is to merge the elements in buffer as much as possible
  // Then, if the number of merged items is larger than rate times total length of buffer (rate*nmemb)
  // Write the buffer into the file_name otherwise return the next free slot in buffer
  //
  
  // i moves over all elemets in the buffer
  // j points to the processing item
  for ( j = 0, i = 1 ; i < nmemb ; i++ ) {
    if ( !compare_matrix_elem( buffer + j , buffer + i ) ) {
      buffer[j].value += buffer[i].value ;
      buffer[i] = (buffer_item_type){0,0,0,0} ;
    }
    else {
      buffer[++j] = buffer[i] ;
    }
  }

  if ( j + 1 > rate*nmemb ) {
    if ( (fid = fopen(file_name, "w")) == NULL ) return -1 ;
    if ( fwrite(buffer, size, j+1, fid) != j+1 ) return -2 ;
    fclose(fid) ;
    return 0 ;
  } 
  return j+1 ;
}


/* Swap two entries of priority queue */
void swap_entry(buffer_item_id_type *pq, int i, int j) {
    buffer_item_id_type temp = pq[i];
    pq[i] = pq[j];
    pq[j] = temp;
}

/* Insert entry into priority queue */
void insert(buffer_item_id_type *pq,  buffer_item_id_type new, int size) {
    int j = size - 1, p;
    pq[j] = new;
    while( ( p = ( j - 1 ) / 2 ) >= 0 ) {
        if( compare_buffer_item_id( pq[p] , pq[j] ) > 0) {
          swap_entry(pq,p,j); 
          j = p;
        }
        else break;
    }
}

/* Delete entry from priority queue */
void delete(buffer_item_id_type *pq, int size) {
    int j, p = 0;
    pq[p] = pq[size - 1];
    while( (j = 2*p+1) < size - 1 ) {
        if(j == size - 2) {
            if(compare_buffer_item_id(pq[p],pq[j]) > 0) swap_entry(pq,p,j);
            return;
        }
        else {
            if(compare_buffer_item_id(pq[j], pq[j+1]) < 0) {
                if(compare_buffer_item_id(pq[p],pq[j]) > 0) {swap_entry(pq,p,j); p = j;}
                else return;
            }
            else {
                if(compare_buffer_item_id(pq[p],pq[j+1]) > 0) {swap_entry(pq,p,j+1); p = j + 1;}
                else return;
            }
        }
    }
}

/* Write top node of priority queue to file, accumulating duplicate entries */
int merge_write(buffer_item_id_type new, buffer_item_id_type *old, FILE *fout) {
    if(new.row == old->row && new.col == old->col && new.hgt == old->hgt) {
        old->value += new.value ;
        return 0; // Indicates duplicate entry
    }
    fwrite(old, sizeof(buffer_item_type), 1, fout);
    *old = new;
    return 1; // Actually wrote to file
}

int matrix_save(char* path, matrix_type* matrix, output_format_type output_format) {
  FILE *outfid, **infid ;
  matrix_index_type h, r, c ;
  matrix_element_type elem ;
  size_t size, i, counter = 0 ;
  char *file_name ;
  buffer_item_id_type *queue, new, old ;

  if ( ( outfid = fopen(path, "w") ) == NULL ) {
    sprintf(ERROR_MESSAGE ,"save_matrix: could not create %s!", path) ;
    return -3 ;
  }

  for ( h = 0 ; h < matrix->hgt ; h++ ) {

    // print the dense part
    for (c = 0 ; c < matrix->col ; c++) {
      for (r = 0 ; r < matrix->array_length[c] ; r++) {
        elem = *(matrix->layer[h].array[c] + r) ;
        if (elem > 0) {
          if (output_format == BIN) {
            fwrite(&r, sizeof(matrix_index_type), 1, outfid) ;
            fwrite(&c, sizeof(matrix_index_type), 1, outfid) ;
            fwrite(&h, sizeof(matrix_index_type), 1, outfid) ;
            fwrite(&elem, sizeof(matrix_element_type), 1, outfid) ;
          }
          //else if (output_format == TXT) 
            //fprintf(fid, format, r, c, h, elem) ;
        }
      }
    }

    // make the buffer empty
    if ( matrix->layer[h].ind > 0 ) {
      qsort(matrix->layer[h].buffer, matrix->layer[h].ind, sizeof(buffer_item_type), compare_matrix_elem) ; 
      file_name = (char*)my_malloc(strlen(matrix->overflow_file) + 100) ;
      sprintf(file_name, "%s_%zd_%zd.bin", matrix->overflow_file, h, matrix->layer[h].overflow_file_counter) ;
      matrix->layer[h].ind = write_buffer(matrix->layer[h].buffer, sizeof(buffer_item_type), matrix->layer[h].ind, file_name, 0) ; 
      free(file_name) ;
      assert (matrix->layer[h].ind == 0) ;
      matrix->layer[h].overflow_file_counter++ ;
      if (VERBOSE) fprintf(stderr, "Layer %zd: number of overflow_files %zd\n", h, matrix->layer[h].overflow_file_counter) ;
    }


    if ( matrix->layer[h].overflow_file_counter > 0) {
      infid = (FILE**)my_malloc(sizeof(FILE*) * matrix->layer[h].overflow_file_counter) ;
      queue = (buffer_item_id_type*)my_malloc(sizeof(buffer_item_id_type) * matrix->layer[h].overflow_file_counter) ;
      for ( i = 0 ; i < (int)(matrix->layer[h].overflow_file_counter) ; i++ ) {
        file_name = (char*)my_malloc(strlen(matrix->overflow_file) + 100) ;
        sprintf(file_name, "%s_%zd_%zd.bin", matrix->overflow_file, h, i) ;
        if ( ( infid[i] = fopen(file_name, "rb") ) == NULL ) {
          sprintf(ERROR_MESSAGE ,"save_matrix: could not find %s!", file_name) ;
          return -1 ;
        }
        if ( fread( &new, sizeof(buffer_item_type), 1, infid[i] ) != 1 ) {
          sprintf(ERROR_MESSAGE ,"save_matrix: could not read the head of  %s!", file_name) ;
          return -2 ;
        }
        new.id = i ;
        insert(queue, new, i + 1) ;
      }

      /* Pop top node, save it in old to see if the next entry is a duplicate */
      size = matrix->layer[h].overflow_file_counter ;
      old = queue[0];
      i = old.id ;
      delete(queue, size);
      fread(&new, sizeof(buffer_item_type), 1, infid[i]);
      if(feof(infid[i])) size--;
      else {
          new.id = i;
          insert(queue, new, size);
      }

      if(VERBOSE) fprintf(stderr, "Merging cooccurrence files: processed 0 lines.");
      /* Repeatedly pop top node and fill priority queue until files have reached EOF */
      while(size > 0) {
          counter += merge_write(queue[0], &old, outfid); // Only count the lines written to file, not duplicates
          if((counter%100000) == 0) if(VERBOSE) fprintf(stderr,"\033[39G%zd lines.",counter);
          i = queue[0].id;
          delete(queue, size);
          fread(&new, sizeof(buffer_item_type), 1, infid[i]);
          if(feof(infid[i])) { size--; fclose(infid[i]); }
          else {
              new.id = i;
              insert(queue, new, size);
          }
      }
      fwrite(&old, sizeof(buffer_item_type), 1, outfid);
      free(queue) ;
      free(infid) ;

      if(VERBOSE) fprintf(stderr,"\033[39G%zd lines.\n",counter);
      if(VERBOSE) fprintf(stderr,"Removing temporary files ...\n") ;
      for ( i = 0 ; i < (int)(matrix->layer[h].overflow_file_counter) ; i++ ) {
        file_name = (char*)my_malloc(strlen(matrix->overflow_file) + 100) ;
        sprintf(file_name, "%s_%zd_%zd.bin", matrix->overflow_file, h, i) ;
        remove(file_name) ;
      }
    }
  }
  fclose(outfid) ;

  return 0 ;
}
#endif


matrix_element_type matrix_add_elem(matrix_type* matrix, matrix_index_type r, matrix_index_type c, matrix_index_type h, matrix_element_type v) {
  matrix_element_type value ;
#ifdef _USE_LINKED_LIST_    
  column_list_type *cprv, *ctmp ;
#else
  char *file_name ;  
  int status ;
#endif

  //fprintf(stderr,"(%zu,%zu,%zu) (%zu,%zu,%zu) \n",r,c,h,matrix->row,matrix->col,matrix->hgt) ;
  assert((r < matrix->row) && (r >= 0)) ;
  assert((c < matrix->col) && (c >= 0)) ;
  assert((h < matrix->hgt) && (h >= 0)) ;

  if (r < matrix->array_length[c]) {
    matrix->hit++ ;
    return (*(matrix->layer[h].array[c] + r) += v) ;
  }
  else {
#ifdef _USE_LINKED_LIST_    
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
    value = ctmp->value ;
#else
    matrix->layer[h].buffer[matrix->layer[h].ind++] = (buffer_item_type) { r, c, h, v } ;
    if (matrix->layer[h].ind == matrix->layer[h].buffer_size) {
     qsort(matrix->layer[h].buffer, matrix->layer[h].ind, sizeof(buffer_item_type), compare_matrix_elem) ; 
     file_name = (char*)my_malloc(strlen(matrix->overflow_file) + 100) ;
     sprintf(file_name, "%s_%zd_%zd.bin", matrix->overflow_file, h, matrix->layer[h].overflow_file_counter) ;
     if ( ( status = write_buffer(matrix->layer[h].buffer, sizeof(buffer_item_type), matrix->layer[h].ind, file_name, 0) ) < 0 ) {
       sprintf(ERROR_MESSAGE, "matrix_elem_add: file operation failed! [status=%d]", status) ;
       return -1 ;
     }
     matrix->layer[h].ind = status ;
     if (status == 0) matrix->layer[h].overflow_file_counter++ ; 
     free(file_name) ;
    }
    value = matrix->layer[h].buffer[matrix->layer[h].ind-1].value ;
#endif
    matrix->miss_hit++ ;
  }
  return value ;
}

matrix_element_type matrix_get_elem(matrix_type* matrix, matrix_index_type r, matrix_index_type c, matrix_index_type h) {

#ifdef _USE_LINKED_LIST_    
  column_list_type *ctmp = NULL ;
#endif 
  assert((r < matrix->row) && (r >= 0)) ;
  assert((c < matrix->col) && (c >= 0)) ;
  assert((h < matrix->hgt) && (h >= 0)) ;

  if (r < matrix->array_length[c])
    return *(matrix->layer[h].array[c] + r) ;
#ifdef _USE_LINKED_LIST_    
  else 
    for (ctmp = matrix->layer[h].list[c] ; ctmp && (ctmp->row != r); ctmp = ctmp->next) ;
  return ctmp ? ctmp->value : 0.0 ;
#endif
  return 0 ;
}
#endif
