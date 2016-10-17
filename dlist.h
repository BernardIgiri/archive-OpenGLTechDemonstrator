/*****************************************************************************
*                                                                            *
*  ------------------------------- dlist.h --------------------------------  *
*                                                                            *
*****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <stdlib.h>

/*****************************************************************************
*                                                                            *
*  Define a structure for doubly-linked list elements.                       *
*                                                                            *
*****************************************************************************/

typedef struct DListElmt_ {

void               *data;
struct DListElmt_  *prev;
struct DListElmt_  *next;

} DListElmt;

/*****************************************************************************
*                                                                            *
*  Define a structure for doubly-linked lists.                               *
*                                                                            *
*****************************************************************************/

typedef struct DList_ {

int                size;

int                (*compare)(const void *key1, const void *key2, int method);
void               (*destroy)(void *data);

DListElmt          *head;
DListElmt          *tail;

} DList;

/*****************************************************************************
*                                                                            *
*  --------------------------- Public Interface ---------------------------  *
*                                                                            *
*****************************************************************************/

void dlist_init(DList *list, void (*destroy)(void *data), int (*compare)(const void *key1, const void *key2, int method));

void dlist_destroy(DList *list);

void dlist_sort(DList *list, int method, int reverseSort);

int dlist_search(DList *list, void *data, int method, DListElmt **result);

int dlist_ins_next(DList *list, DListElmt *element, const void *data);

int dlist_ins_prev(DList *list, DListElmt *element, const void *data);

int dlist_remove(DList *list, DListElmt *element, void **data);

#define dlist_compare(list,element1,element2,method) ((list)->compare((element1)->data,(element2)->data,method))

#define dlist_compare_data(list,key1,key2,method) ((list)->compare(key1,key2,method))

#define dlist_size(list) ((list)->size)

#define dlist_head(list) ((list)->head)

#define dlist_tail(list) ((list)->tail)

#define dlist_is_head(element) ((element)->prev == NULL ? 1 : 0)

#define dlist_is_tail(element) ((element)->next == NULL ? 1 : 0)

#define dlist_data(element) ((element)->data)

#define dlist_next(element) ((element)->next)

#define dlist_prev(element) ((element)->prev)

#endif
