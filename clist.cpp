/*****************************************************************************
*                                                                            *
*  ------------------------------- clist.c --------------------------------  *
*                                                                            *
*****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "clist.h"

/*****************************************************************************
*                                                                            *
*  ------------------------------ clist_init ------------------------------  *
*                                                                            *
*****************************************************************************/

void clist_init(CList *list, void (*destroy)(void *data), int (*compare)(const void *key1, const void *key2, int method)) {

/*****************************************************************************
*                                                                            *
*  Initialize the list.                                                      *
*                                                                            *
*****************************************************************************/

list->size = 0;
list->destroy = destroy;
list->compare = compare;
list->head = NULL;

return;

}

/*****************************************************************************
*                                                                            *
*  ---------------------------- clist_destroy -----------------------------  *
*                                                                            *
*****************************************************************************/

void clist_destroy(CList *list) {

void               *data;

/*****************************************************************************
*                                                                            *
*  Remove each element.                                                      *
*                                                                            *
*****************************************************************************/

while (clist_size(list) > 0) {

   if (clist_rem_next(list, list->head, (void **)&data) == 0 && list->destroy
      != NULL) {

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

memset(list, 0, sizeof(CList));

return;

}

/*****************************************************************************
*                                                                            *
*  ----------------------------- clist_sort -------------------------------  *
*                                                                            *
*****************************************************************************

// O((n^2)/2+1) operation, correction O(n^(2.1))
void clist_sort(CList *list, int method, int reverseSort)
{
	if (clist_size(list)<2)
		return;
	if (list->compare==NULL)
		return;
	int reverse = (reverseSort ? -1 : 1),
		progress = 0;
	CListElmt *key0=NULL, *key1=clist_head(list), *key2=clist_next(clist_head(list)),
			  *tail=NULL;
	bool done = false;
	unsigned long int count = 0, limit = clist_size(list)*clist_size(list)/2;
	while (!done)
	{
		int comp = clist_compare(list,key1,key2,method)*reverse;
		// make swap if key1 is greater that key2
		if (comp>NULL)
		{
			progress=1;
			if (NULL==key0)
			{
				clist_head(list) = key2;
				if (NULL!=tail)
					clist_next(tail) = clist_head(list); // ensure the tail points to head
			}
			else
				clist_next(key0) = key2;
			clist_next(key1) = clist_next(key2);
			clist_next(key2) = key1;
			//no tail
		}
		// advance
		key0=key1;
		key1=key2;
		key2=clist_next(key2);
		if (clist_is_head(list,clist_next(key1)))
		{
			tail=key1;
			if (count>limit)// prevent infinite loop
				done = true;
			count++;
			if (NULL==progress)
				done=true; // end if last cycle thru showed nothing out of sequential order
			else
			{ // restart at begining reset progress to check status of new pass
				key0=NULL;
				key1=clist_head(list);
				key2=clist_next(clist_head(list));
				progress=0;
			}
		}
	}
}

/*****************************************************************************
*                                                                            *
*  ---------------------------- clist_ins_next ----------------------------  *
*                                                                            *
*****************************************************************************/

int clist_ins_next(CList *list, CListElmt *element, const void *data) {

CListElmt          *new_element;

/*****************************************************************************
*                                                                            *
*  Allocate storage for the element.                                         *
*                                                                            *
*****************************************************************************/

if ((new_element = (CListElmt *)malloc(sizeof(CListElmt))) == NULL)
   return -1;

/*****************************************************************************
*                                                                            *
*  Insert the element into the list.                                         *
*                                                                            *
*****************************************************************************/

new_element->data = (void *)data;

if (clist_size(list) == 0) {

   /**************************************************************************
   *                                                                         *
   *  Handle insertion when the list is empty.                               *
   *                                                                         *
   **************************************************************************/

   new_element->next = new_element;
   list->head = new_element;

   }

else {

   /**************************************************************************
   *                                                                         *
   *  Handle insertion when the list is not empty.                           *
   *                                                                         *
   **************************************************************************/

   new_element->next = element->next;
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
*  ---------------------------- clist_rem_next ----------------------------  *
*                                                                            *
*****************************************************************************/

int clist_rem_next(CList *list, CListElmt *element, void **data) {

CListElmt          *old_element;

/*****************************************************************************
*                                                                            *
*  Do not allow removal from an empty list.                                  *
*                                                                            *
*****************************************************************************/

if (clist_size(list) == 0)
   return -1;

/*****************************************************************************
*                                                                            *
*  Remove the element from the list.                                         *
*                                                                            *
*****************************************************************************/

*data = element->next->data;

if (element->next == element) {

   /**************************************************************************
   *                                                                         *
   *  Handle removing the last element.                                      *
   *                                                                         *
   **************************************************************************/

   old_element = element->next;
   list->head = NULL;

   }

else {

   /**************************************************************************
   *                                                                         *
   *  Handle removing other than the last element.                           *
   *                                                                         *
   **************************************************************************/

   old_element = element->next;
   element->next = element->next->next;

}

/*****************************************************************************
*                                                                            *
*  Free the storage allocated by the abstract data type.                     *
*                                                                            *
*****************************************************************************/

free(old_element);

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

*  ----------------------------- clist_search -----------------------------  *

*                                                                            *

*****************************************************************************/

int clist_search(CList *list, void *data, int method, CListElmt **result)
{
	if (list->compare==NULL)
		return 0;
	if (clist_size(list)==0)
		return 0;
	CListElmt *curElmt = (*result);
	CListElmt *start = curElmt;
	do
	{
		if (clist_compare_data(list,clist_data(curElmt),data,method)==NULL)
		{
			*result=curElmt;
			return 1;
		}
		curElmt = clist_next(curElmt);
	} while (curElmt!=start);
	return 0;
}