/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 **/

#include <octave/oct.h>
#include <iostream>
#include <fstream>
#include "cooc.h"

typedef struct {
  COOC_INDEX_TYPE row ;
  COOC_INDEX_TYPE col ;
  COOC_INDEX_TYPE hgt ;
  COOC_ELEM_TYPE  val ;
} COOC_ELEMENT ;

DEFUN_DLD(bin2oct, args, nargout,"cooc = bin2mat(path)"){
  int nargin = args.length() ;
  Matrix ret ((octave_idx_type)(0), (octave_idx_type)(4)); // row, column, hgt, value

  /* init_hash */
  if (nargin != 1) {
    std::cerr << "invalid use of bin2mat" << std::endl ;
    return octave_value(ret) ;
  }

  std::string path(args(0).string_value()) ;
  
  std::ifstream input(path.c_str(), std::ios::binary );
  if (!input.is_open()) {
    std::cerr << "Unable to open input file " << path << std::endl ;
    return octave_value(ret) ;
  }

  input.seekg(0, input.end) ;
  size_t nelem = input.tellg()/sizeof(COOC_ELEMENT) ;
  input.seekg(0, input.beg) ;

  ret.resize((octave_idx_type)(nelem), (octave_idx_type)(4)) ;
  COOC_ELEMENT elem ;
  octave_idx_type n = 0;

  while(!input.eof() && (n < nelem)) {
    input.read((char*)&elem, sizeof(COOC_ELEMENT)) ;
    ret.elem(n, 0) = elem.row ;
    ret.elem(n, 1) = elem.col ;
    ret.elem(n, 2) = elem.hgt ;
    ret.elem(n, 3) = elem.val ;
    n++ ;
  }
  input.close() ;
  return octave_value(ret) ;
}


