/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 **/

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
