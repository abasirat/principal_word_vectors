#ifndef __TEST_C
#define __TEST_C 

#include "test.h"

int f(int a) {
  return a++ ;
}

#endif

int main(int argc, char* argv[]) {
  return f(1) ;
}
