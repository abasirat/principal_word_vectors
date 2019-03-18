#include <stdio.h>
#include "matrix.h"


int main(int argc, char* argv[]) {
  FILE *fp ;
  buffer_item_type item ;
  if ( ( fp = fopen(argv[1], "r") ) == NULL ) {
    fprintf(stderr, "Unable to open %s\n", argv[1]) ;
    return 1 ;
  }

  while(!feof(fp)) {
    fread(&item, sizeof(buffer_item_type), 1, fp) ;
    fprintf(stdout, "%zd %zd %zd %e\n", item.row, item.col, item.hgt, item.value) ;
  }
  fclose(fp) ;
  return 0 ;
}
