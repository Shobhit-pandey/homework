#ifndef __LIST_H
#define __LIST_H

/*
   File: list.h
   Interface definition of the list data structure.
   CS 261
*/

#include "type.h"
struct DLink {
  KeyType       key;
  ValueType     val;
  struct DLink *prev;
  struct DLink *next;
};
typedef struct DLink DLink;

struct List {
  struct DLink *head;
  struct DLink *tail;
  int           count;
};
typedef struct List List;

/* Initialize linked list structure. */
void initList(struct List *l);

/* Allocate and initialize linked list structure. */
struct List *newList();

/* Deallocate links in linked list. */
void clearList(struct List *l);

/* Deallocate links and linked list structure. */
void  freeList(struct List *l);

/* Stack interface. */
int isEmptyList(struct List *l);

void	pushList(struct List *l, KeyType k, ValueType v);

ValueType	topList(struct List *l);

void	popList(struct List *l);

/* Bag interface. */
int	sizeList(struct List *l);

void	addList(struct List *l, KeyType k, ValueType v);

int	containsList(struct List *l, KeyType k);

void	removeList(struct List *l, KeyType k);

ValueType* getList(struct List *l, KeyType k);

/* Utility Functions */
void printList(struct List *l);
#endif

