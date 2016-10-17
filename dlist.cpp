/*****************************************************************************
*                                                                            *
*  ------------------------------- dlist.c --------------------------------  *
*                                                                            *
*****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "dlist.h"

/*****************************************************************************
*                                                                            *
*  ------------------------------ dlist_init ------------------------------  *
*                                                                            *
*****************************************************************************/

void dlist_init(DList *list, void (*destroy)(void *data),int (*compare)(const void *key1, const void *key2, int method)) {

/*****************************************************************************
*                                                                            *
*  Initialize the list.                                                      *
*                                                                            *
*****************************************************************************/

list->size = 0;
list->destroy = destroy;
list->compare = compare;
list->head = NULL;
list->tail = NULL;

return;

}

/*****************************************************************************
*                                                                            *
*  ---------------------------- dlist_destroy -----------------------------  *
*                                                                            *
*****************************************************************************/

void dlist_destroy(DList *list) {

void               *data;

/*****************************************************************************
*                                                                            *
*  Remove each element.                                                      *
*                                                                            *
*****************************************************************************/

while (dlist_size(list) > 0) {

   if (dlist_remove(list, dlist_tail(list), (void **)&data) == 0 && list->
      destroy != NULL) {

      /***********************************************************************
      *                                                                      *
      *  Call a user-defined function to free dynamically allocated data.    *
      *                                                                      *
      ***********************************************************************/

      list->destroy(data);

   }

}

/*****************************************************************************
*                                                                            *
*  No operations are allowed now, but clear the structure as a precaution.   *
*                                                                            *
*****************************************************************************/

memset(list, 0, sizeof(DList));

return;

}

/*****************************************************************************
*                                                                            *
*  ---------------------------- dlist_ins_next ----------------------------  *
*                                                                            *
*****************************************************************************/

int dlist_ins_next(DList *list, DListElmt *element, const void *data) {

DListElmt          *new_element;

/*****************************************************************************
*                                                                            *
*  Do not allow a NULL element unless the list is empty.                     *
*                                                                            *
*****************************************************************************/

if (element == NULL && dlist_size(list) != 0)
   return -1;

/*****************************************************************************
*                                                                            *
*  Allocate storage for the element.                                         *
*                                                                            *
*****************************************************************************/

if ((new_element = (DListElmt *)malloc(sizeof(DListElmt))) == NULL)
   return -1;

/*****************************************************************************
*                                                                            *
*  Insert the new element into the list.                                     *
*                                                                            *
*****************************************************************************/

new_element->data = (void *)data;

if (dlist_size(list) == 0) {

   /**************************************************************************
   *                                                                         *
   *  Handle insertion when the list is empty.                               *
   *                                                                         *
   **************************************************************************/

   list->head = new_element;
   list->head->prev = NULL;
   list->head->next = NULL;
   list->tail = new_element;

   }

else {

   /**************************************************************************
   *                                                                         *
   *  Handle insertion when the list is not empty.                           *
   *                                                                         *
   **************************************************************************/

   new_element->next = element->next;
   new_element->prev = element;

   if (element->next == NULL)
      list->tail = new_element;
   else
      element->next->prev = new_element;

   element->next = new_element;

}

/*****************************************************************************
*                                                                            *
*  Adjust the size of the list to account for the inserted element.          *
*                                                                            *
*****************************************************************************/

list->size++;

return 0;

}

/*****************************************************************************
*                                                                            *
*  ---------------------------- dlist_ins_prev ----------------------------  *
*                                                                            *
*****************************************************************************/


int dlist_ins_prev(DList *list, DListElmt *element, const void *data) {

DListElmt          *new_element;

/*****************************************************************************
*                                                                            *
*  Do not allow a NULL element unless the list is empty.                     *
*                                                                            *
*****************************************************************************/

if (element == NULL && dlist_size(list) != 0)
   return -1;

/*****************************************************************************
*                                                                            *
*  Allocate storage to be managed by the abstract data type.                 *
*                                                                            *
*****************************************************************************/

if ((new_element = (DListElmt *)malloc(sizeof(DListElmt))) == NULL)
   return -1;

/*****************************************************************************
*                                                                            *
*  Insert the new element into the list.                                     *
*                                                                            *
*****************************************************************************/

new_element->data = (void *)data;

if (dlist_size(list) == 0) {

   /**************************************************************************
   *                                                                         *
   *  Handle insertion when the list is empty.                               *
   *                                                                         *
   **************************************************************************/

   list->head = new_element;
   list->head->prev = NULL;
   list->head->next = NULL;
   list->tail = new_element;

   }


else {

   /**************************************************************************
   *                                                                         *
   *  Handle insertion when the list is not empty.                           *
   *                                                                         *
   **************************************************************************/

   new_element->next = element; 
   new_element->prev = element->prev;

   if (element->prev == NULL)
      list->head = new_element;
   else
      element->prev->next = new_element;

   element->prev = new_element;

}


/*****************************************************************************
*                                                                            *
*  Adjust the size of the list to account for the new element.               *
*                                                                            *
*****************************************************************************/

list->size++;

return 0;

}

/*****************************************************************************
*                                                                            *
*  ----------------------------- dlist_remove -----------------------------  *
*                                                                            *
*****************************************************************************/

int dlist_remove(DList *list, DListElmt *element, void **data) {

/*****************************************************************************
*                                                                            *
*  Do not allow a NULL element or removal from an empty list.                *
*                                                                            *
*****************************************************************************/

if (element == NULL || dlist_size(list) == 0)
   return -1;

/*****************************************************************************
*                                                                            *
*  Remove the element from the list.                                         *
*                                                                            *
*****************************************************************************/

*data = element->data;

if (element == list->head) {

   /**************************************************************************
   *                                                                         *
   *  Handle removal from the head of the list.                              *
   *                                                                         *
   **************************************************************************/

   list->head = element->next;

   if (list->head == NULL)
      list->tail = NULL;
   else
      element->next->prev = NULL;

   }

else {

   /**************************************************************************
   *                                                                         *
   *  Handle removal from other than the head of the list.                   *
   *                                                                         *
   **************************************************************************/

   element->prev->next = element->next;

   if (element->next == NULL)
      list->tail = element->prev;
   else
      element->next->prev = element->prev;

}

/*****************************************************************************
*                                                                            *
*  Free the storage allocated by the abstract data type.                     *
*                                                                            *
*****************************************************************************/

free(element);

/*****************************************************************************
*                                                                            *
*  Adjust the size of the list to account for the removed element.           *
*                                                                            *
*****************************************************************************/

list->size--;

return 0;

}

/*****************************************************************************

*                                                                            *

*  ------------------------------ dlist_sort ------------------------------  *

*                                                                            *

*****************************************************************************/

// O((n^2)/2+1) operation, correction O(n^(2.1))
void dlist_sort(DList *list, int method, int reverseSort)
{
	if (dlist_size(list)<2)
		return;
	if (list->compare==NULL)
		return;
	int reverse = (reverseSort ? -1 : 1),
		progress = 0;
	DListElmt *key1=dlist_head(list), *key2=dlist_next(dlist_head(list));
	bool done = false;
	unsigned long int count = 0, limit = dlist_size(list)*dlist_size(list)/2;
	while (!done)
	{
		int comp = dlist_compare(list,key1,key2,method)*reverse;
		// make swap if key1 is greater that key2
		if (comp>NULL)
		{
			progress=1;
			if (dlist_prev(key1)==NULL)
				dlist_head(list) = key2;
			else
				dlist_next(dlist_prev(key1))=key2;
			dlist_prev(key2)=dlist_prev(key1);
			dlist_prev(key1)=key2;
			if (dlist_next(key2)==NULL)
				dlist_tail(list)=key1;
			else
				dlist_prev(dlist_next(key2))=key1;
			dlist_next(key1)=dlist_next(key2);			
			dlist_next(key2)=key1;
		}
		// advance
		key1=key2;
		key2=dlist_next(key2);
		if (dlist_is_tail(key1))
		{			
			if (count++>limit)// prevent infinite loop
				done = true;
			if (NULL==progress)
				done=true; // end if last cycle thru showed nothing out of sequential order
			else
			{ // restart at begining reset progress to check status of new pass
				key1=dlist_head(list);
				key2=dlist_next(dlist_head(list));
				progress=0;
			}
		}
	}
}

/*****************************************************************************

*                                                                            *

*  ----------------------------- dlist_search -----------------------------  *

*                                                                            *

*****************************************************************************/

int dlist_search(DList *list, void *data, int method, DListElmt **result)
{
	if (dlist_size(list)==0)
		return 0;
	DListElmt *curElmt = dlist_head(list);
	do
	{
		if (dlist_compare_data(list,dlist_data(curElmt),data,method)==NULL)
		{
			*result=curElmt;
			return 1;
		}
		curElmt = dlist_next(curElmt);
	} while (curElmt!=NULL);
	return 0;
}