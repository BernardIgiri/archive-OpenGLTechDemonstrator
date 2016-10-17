/*****************************************************************************
*                                                                            *
*  ------------------------------- clist.h --------------------------------  *
*                                                                            *
*****************************************************************************/

#ifndef CLIST_H
#define CLIST_H

#include <stdlib.h>

/*****************************************************************************
*                                                                            *
*  Define a structure for circular list elements.                            *
*                                                                            *
*****************************************************************************/

typedef struct CListElmt_ {

void               *data;
struct CListElmt_  *next;

} CListElmt;

/*****************************************************************************
*                                                                            *
*  Define a structure for circular lists.                                    *
*                                                                            *
*****************************************************************************/

typedef struct CList_ {

int                size;

int                (*compare)(const void *key1, const void *key2, int method);
void               (*destroy)(void *data);

CListElmt          *head;

} CList;

/*****************************************************************************
*                                                                            *
*  --------------------------- Public Interface ---------------------------  *
*                                                                            *
*****************************************************************************/

//void clist_sort(CList *list, int method, int reverseSort);

void clist_init(CList *list, void (*destroy)(void *data), int (*compare)(const void *key1, const void *key2, int method));

void clist_destroy(CList *list);

int clist_ins_next(CList *list, CListElmt *element, const void *data);

int clist_rem_next(CList *list, CListElmt *element, void **data);

int clist_search(CList *list, void *data, int method, CListElmt **result);

#define clist_size(list) ((list)->size)

#define clist_head(list) ((list)->head)

#define clist_is_head(list, element) ((element) == (list)->head ? 1 : 0)

#define clist_data(element) ((element)->data)

#define clist_next(element) ((element)->next)

#define clist_compare(list,element1,element2,method) ((list)->compare((element1)->data,(element2)->data,method))

#define clist_compare_data(list,key1,key2,method) ((list)->compare(key1,key2,method))

#endif
