/*
   File: list.c
   Implementation of the linked list data structure.
   CS 261
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"
#include "type.h"

/*----------------------------------------------------------------------------*/
void initList(struct List *l) {
  /* Sentinel at the head of the list. */
  l->head = (struct DLink *)malloc(sizeof(struct DLink));
  assert(l->head != 0);

  /* Sentinel at the tail of the list. */
  l->tail = (struct DLink *)malloc(sizeof(struct DLink));
  assert(l->head != 0);

  l->head->prev = 0;  
  l->head->next = l->tail;
  l->tail->next = 0;  
  l->tail->prev = l->head;

  l->count = 0;
}
/*----------------------------------------------------------------------------*/
struct List *newList() {
  struct List *list = (struct List *)malloc(sizeof(struct List));

  initList(list);

  return list;
}
/*----------------------------------------------------------------------------*/
void clearList(struct List *l) {
  struct DLink *link;

  while (l->head != 0) { link = l->head;  l->head = link->next;  free(link); }
}
/*----------------------------------------------------------------------------*/
void freeList(struct List *l) {
  clearList(l);
  free(l);
}
/*----------------------------------------------------------------------------*/

/*
   Stack interface.
*/
/*----------------------------------------------------------------------------*/
int isEmptyList(struct List *l) { return (l->count == 0); }
/*----------------------------------------------------------------------------*/
void _addLink(struct DLink *l, KeyType k, ValueType v) {
  struct DLink *link = (struct DLink *)malloc(sizeof(struct DLink));
  assert(link != 0);

  link->key  = k;
  link->val  = v;

  link->next = l;
  link->prev = l->prev;

  link->next->prev = link;
  link->prev->next = link;
}
/*----------------------------------------------------------------------------*/
void pushList(struct List *l, KeyType k, ValueType v) {

 _addLink(l->head->next, k, v);  
  l->count++;
}
/*----------------------------------------------------------------------------*/
ValueType topList(struct List *l) {
  assert(!isEmptyList(l));
  return l->head->next->val;
}
/*----------------------------------------------------------------------------*/
void _removeLink(struct DLink *l) {
  l->prev->next = l->next;
  l->next->prev = l->prev;
  free(l);
}
/*----------------------------------------------------------------------------*/
void popList(struct List *l) {
  assert(!isEmptyList(l));
 _removeLink(l->head->next);
  l->count--;
}
/*----------------------------------------------------------------------------*/

/*
   Bag interface.
*/
/*----------------------------------------------------------------------------*/
int sizeList(struct List *l) { return l->count; }
/*----------------------------------------------------------------------------*/
void addList(struct List *l, KeyType k, ValueType v) {
  /* You need to write this function. */
  /* Can add to any location...so just add to the front of a stack */
  pushList(l, k, v);  
}
/*----------------------------------------------------------------------------*/
int containsList(struct List *l, KeyType k) {
  /* You need to write this function. */
  struct DLink *link = l->head->next;

  while(link != l->tail)
  {
    if(EQ(link->key, k)) {
	    return 1;
	}
    link = link->next;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
ValueType* getList(struct List *l, KeyType k) {
  /* You need to write this function. */
  struct DLink *link = l->head->next;

  while(link != l->tail)
  {
    if(EQ(link->key, k)) {
	    return &link->val;
    }
    link = link->next;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
void removeList(struct List *l, KeyType k) {
  /* You need to write this function. */
  struct DLink *link = l->head->next;

  while(link != l->tail)
  {
    if(EQ(link->key, k))
	{
	   _removeLink(link);
	   l->count--;
	}
    link = link->next;
  }
}
/*----------------------------------------------------------------------------*/

/*
    Utility Functions
*/
/*----------------------------------------------------------------------------*/
void printList(struct List *l) {
    DLink* tmp;
    tmp = l->head->next;
    while( tmp != l->tail ) {
        printf("%s:%d\n", tmp->key, tmp->val);
        tmp = tmp->next;
    }
}
/*----------------------------------------------------------------------------*/

