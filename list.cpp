/*****************************************************************************

*                                                                            *

*  -------------------------------- list.c --------------------------------  *

*                                                                            *

*****************************************************************************/



#include <stdlib.h>

#include <string.h>



#include "list.h"



/*****************************************************************************

*                                                                            *

*  ------------------------------- list_init ------------------------------  *

*                                                                            *

*****************************************************************************/



void list_init(List *list, void (*destroy)(void *data), int (*compare)(const void *key1, const void *key2, int method)) {



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

*  ----------------------------- list_destroy -----------------------------  *

*                                                                            *

*****************************************************************************/



void list_destroy(List *list) {



void               *data;



/*****************************************************************************

*                                                                            *

*  Remove each element.                                                      *

*                                                                            *

*****************************************************************************/



while (list_size(list) > 0) {



   if (list_rem_next(list, NULL, (void **)&data) == 0 && list->destroy !=

      NULL) {



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



memset(list, 0, sizeof(List));



return;



}


/*****************************************************************************

*                                                                            *

*  ------------------------------ list_sort -------------------------------  *

*                                                                            *

*****************************************************************************/

// O((n^2)/2+1) operation, correction O(n^(2.1))
void list_sort(List *list, int method, int reverseSort)
{
	if (list_size(list)<2)
		return;
	if (list->compare==NULL)
		return;
	int reverse = (reverseSort ? -1 : 1),
		progress = 0;
	ListElmt *key0=NULL, *key1=list_head(list), *key2=list_next(list_head(list));
	bool done = false;
	unsigned long int count = 0, limit = list_size(list)*list_size(list)/2;
	while (!done)
	{
		int comp = list_compare(list,key1,key2,method)*reverse;
		// make swap if key1 is greater that key2
		if (comp>NULL)
		{
			progress=1;
			if (NULL==key0)
				list_head(list) = key2;
			else
				list_next(key0) = key2;
			list_next(key1) = list_next(key2);
			list_next(key2) = key1;
			if (list_is_tail(key1)) // if now the tail set list tail to point to it
				list_tail(list)=key1;
		}
		// advance
		key0=key1;
		key1=key2;
		key2=list_next(key2);
		if (list_is_tail(key1))
		{			
			if (count++>limit)// prevent infinite loop
				done = true;
			if (NULL==progress)
				done=true; // end if last cycle thru showed nothing out of sequential order
			else
			{ // restart at begining reset progress to check status of new pass
				key0=NULL;
				key1=list_head(list);
				key2=list_next(list_head(list));
				progress=0;
			}
		}
	}
}

/*****************************************************************************

*                                                                            *

*  ----------------------------- list_ins_next ----------------------------  *

*                                                                            *

*****************************************************************************/



int list_ins_next(List *list, ListElmt *element, const void *data) {



ListElmt           *new_element;



/*****************************************************************************

*                                                                            *

*  Allocate storage for the element.                                         *

*                                                                            *

*****************************************************************************/



if ((new_element = (ListElmt *)malloc(sizeof(ListElmt))) == NULL)

   return -1;



/*****************************************************************************

*                                                                            *

*  Insert the element into the list.                                         *

*                                                                            *

*****************************************************************************/



new_element->data = (void *)data;



if (element == NULL) {



   /**************************************************************************

   *                                                                         *

   *  Handle insertion at the head of the list.                              *

   *                                                                         *

   **************************************************************************/



   if (list_size(list) == 0)

      list->tail = new_element;



   new_element->next = list->head;

   list->head = new_element;



   }



else {



   /**************************************************************************

   *                                                                         *

   *  Handle insertion somewhere other than at the head.                     *

   *                                                                         *

   **************************************************************************/



   if (element->next == NULL)

      list->tail = new_element;



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

*  ----------------------------- list_rem_next ----------------------------  *

*                                                                            *

*****************************************************************************/



int list_rem_next(List *list, ListElmt *element, void **data) {



ListElmt           *old_element;



/*****************************************************************************

*                                                                            *

*  Do not allow removal from an empty list.                                  *

*                                                                            *

*****************************************************************************/



if (list_size(list) == 0)

   return -1;



/*****************************************************************************

*                                                                            *

*  Remove the element from the list.                                         *

*                                                                            *

*****************************************************************************/



if (element == NULL) {



   /**************************************************************************

   *                                                                         *

   *  Handle removal from the head of the list.                              *

   *                                                                         *

   **************************************************************************/



   *data = list->head->data;

   old_element = list->head;

   list->head = list->head->next;



   if (list_size(list) == 1)

      list->tail = NULL;



   }



else {



   /**************************************************************************

   *                                                                         *

   *  Handle removal from somewhere other than the head.                     *

   *                                                                         *

   **************************************************************************/



   if (element->next == NULL)

      return -1;



   *data = element->next->data;

   old_element = element->next;

   element->next = element->next->next;



   if (element->next == NULL)

      list->tail = element;



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

*  ------------------------------ list_search -----------------------------  *

*                                                                            *

*****************************************************************************/

int list_search(List *list, void *data, int method, ListElmt **result)
{
	if (list_size(list)==0)
		return 0;
	ListElmt *curElmt = list_head(list);
	do
	{
		if (list_compare_data(list,list_data(curElmt),data,method)==NULL)
		{
			*result=curElmt;
			return 1;
		}
		curElmt = list_next(curElmt);
	} while (curElmt!=NULL);
	return 0;
}