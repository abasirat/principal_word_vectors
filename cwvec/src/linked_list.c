#include <stdlib.h>
#include <stdio.h>

typedef struct linked_list_item {
  void *item ;
  struct linked_list_item *next ;
} linked_list ;

linked_list* init_linked_list() {
  linked_list *node = NULL ;
  return node;
}


linked_list* add_to_linked_list(linked_list **list, void *item) {
  linked_list *current, *node ;

  if ( !(node = (linked_list*)malloc(sizeof(linked_list))) ) return NULL ;
  node->item = item ;
  node->next = NULL ;

  if (*list == NULL) *list = node ;
  else {
    for (current = *list ; current->next ; current = current->next) ;
    current->next = node ;
  }
  return node ;
}

void delete_linked_list( linked_list **list, void (*free_item)(void*) ) {
  linked_list *next ;
  while(*list) {
    next = (*list)->next ;

    (*free_item)((*list)->item) ;
    free(*list) ;

    *list = next ;
  }
}
    
/*int main(int argc, char* argv[]) {
  linked_list *l1 = init_linked_list() ;
  linked_list *i ;
  int a , *aa;
  for (a = 0 ; a < 50000 ; a++) {
    aa = (int*)malloc(sizeof(int)) ;
    *aa = a ;
    i = add_to_linked_list(&l1, aa) ;
    //fprintf(stderr, "%x\t%x\t%d\n", l1, i->item, *(int*)(i->item)) ;
  }

  delete_linked_list(&l1, &free) ;
  //fprintf(stderr, "++ %x ++\n", l1) ;
  return 1 ;
}*/


