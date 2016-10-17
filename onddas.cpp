#include "onddas.h"
#include <stdlib.h>
#include <string.h>
/***********************************************************************************************

  Creates an instance of ONDDAS and initalizes variables.

***********************************************************************************************/
ONDDAS::ONDDAS()
{
	m_treeCount		 =0;
	m_dataTypeCount	 =0;
	m_treeList		 =NULL;
	m_dataTypeList	 =NULL;
	m_firstTime		 =true;
}
/***********************************************************************************************

  Destroys instance of ONDDAS and calls Shutdown() if memory hasn't been deallocated.

***********************************************************************************************/
ONDDAS::~ONDDAS()
{
	if (m_treeList!=NULL || m_dataTypeList!=NULL || m_treeCount!=0 || m_dataTypeCount!=0)
		Shutdown();
}
/***********************************************************************************************

  Initializes or Resets System for use.
  Set ComparePtr & DestroyElmtPtr to the Compare & DestroyElmt member functions.

***********************************************************************************************/
bool ONDDAS::Initialize()
{
	try
	{
		if (m_treeList!=NULL || m_dataTypeList!=NULL)
			Shutdown();
		m_treeList = (ONDDAS_Tree *)malloc(sizeof(ONDDAS_Tree)*2);
		if (m_treeList==NULL)
			return false;
		m_dataTypeList = (ONDDAS_DataType *)malloc(sizeof(ONDDAS_DataType)*2);
		if (m_dataTypeList==NULL)
			return false;
		m_treeCount = m_dataTypeCount = 0;
		m_firstTime	=true;
	}
	catch (...)
	{
		throw;
	}
	return true;
}
/***********************************************************************************************

  Registers a new dataType for usage within the system and returns the ID number assigned
  to the dataType. Returns -1 if it fails.

  loadData - should be able to load the requested data specified by the id into ram and return
  a void pointer to the data.  Should return NULL on failure.
  NOTE: If you do not want the system to perfom auto loads set loadData to
  ONDDASNullLoadDataI for by integer id or ONDDASNullLoadDataC for by char* id.

  destroyData - should be able to receive a void pointer to the data and free all of the
  memory allocated in loadData.  Set to NULL if data is not dynamic and cannot be destroyed.

***********************************************************************************************/
int ONDDAS::RegisterDataType(void*(*loadData)(int),void(*destroyData)(void*))
{
	m_dataTypeCount++;
	if (m_dataTypeCount>2)
	{
		try
		{
			ONDDAS_DataType *newptr=NULL;
			newptr = (ONDDAS_DataType *)realloc(m_dataTypeList,sizeof(ONDDAS_DataType)*m_dataTypeCount);
			if (newptr==NULL)
				return -1;
			m_dataTypeList=newptr;
		}
		catch (...)
		{
			throw;
		}
	}
	int n=m_dataTypeCount-1;
	m_dataTypeList[n].method			=ONDDAS_METHOD_NUMBER;
	m_dataTypeList[n].destroyData		=destroyData;
	m_dataTypeList[n].loadData.loadDataI=loadData;
	return n;
}
/***********************************************************************************************

  Registers a new dataType for usage within the system and returns the ID number assigned
  to the dataType. Returns -1 if it fails.

  loadData - should be able to load the requested data specified by the id into ram and return
  a void pointer to the data.  Should return NULL on failure.
  NOTE: If you do not want the system to perfom auto loads set loadData to
  ONDDASNullLoadDataI for by integer id or ONDDASNullLoadDataC for by char* id.

  destroyData - should be able to receive a void pointer to the data and free all of the
  memory allocated in loadData.  Set to NULL if data is not dynamic and cannot be destroyed.

***********************************************************************************************/
int ONDDAS::RegisterDataType(void*(*loadData)(char*),void(*destroyData)(void*))
{
	m_dataTypeCount++;
	if (m_dataTypeCount>2)
	{
		try
		{
			ONDDAS_DataType *newptr=NULL;
			newptr = (ONDDAS_DataType *)realloc(m_dataTypeList,sizeof(ONDDAS_DataType)*m_dataTypeCount);
			if (newptr==NULL)
				return -1;
			m_dataTypeList=newptr;
		}
		catch (...)
		{
			throw;
		}
	}
	int n=m_dataTypeCount-1;
	m_dataTypeList[n].method			=ONDDAS_METHOD_NAME;
	m_dataTypeList[n].destroyData		=destroyData;
	m_dataTypeList[n].loadData.loadDataC=loadData;
	return n;
}
/***********************************************************************************************

  Adds a new tree to the system for the specified data type.
  WARNING: Use at minimum one tree per data type & id method. (id by char* or int)
  Returns the ID number of the new tree.  Returns -1 if it fails.
  NOTE: Register the data type first.

***********************************************************************************************/
int ONDDAS::StartNewTree(int dataTypeID)
{
	if (dataTypeID<0 || dataTypeID>=m_dataTypeCount)
		return -1;
	m_treeCount++;
	if (m_treeCount>2)
	{
		try
		{
			ONDDAS_Tree *newptr=NULL;
			newptr = (ONDDAS_Tree *)realloc(m_treeList,sizeof(ONDDAS_Tree)*m_treeCount);
			if (newptr==NULL)
				return -1;
			m_treeList=newptr;
		}
		catch (...)
		{
			throw;
		}
	}
	int n=m_treeCount-1;
	try
	{
		m_treeList[n].dataTypeID=dataTypeID;
		m_treeList[n].tree		=NULL;
		m_treeList[n].tree	    =new BisTree;
		if (m_treeList[n].tree==NULL)
			return -1;
		bistree_init(m_treeList[n].tree,ONDDASCompare,ONDDASDestroyElmt);
	}
	catch (...)
	{
		throw;
	}
	return n;
}
/***********************************************************************************************

  Shuts down system clearing all of the allocated memory.

***********************************************************************************************/
void ONDDAS::Shutdown()
{
	try
	{
		if (m_dataTypeList!=NULL)
			free(m_dataTypeList);
		m_dataTypeList =NULL;
		m_dataTypeCount=0;
		if (m_treeList!=NULL)
		{
			for (int i=0; i<m_treeCount; i++)
				if (m_treeList[i].tree!=NULL)
				{
					bistree_destroy(m_treeList[i].tree);
					delete m_treeList[i].tree;
				}
			free(m_treeList);
		}
		m_treeList =NULL;
		m_treeCount=0;
	}
	catch (...)
	{
		throw;
	}
}
/***********************************************************************************************

  Attempts to reset a tree by destoying it and its data and remaking it as an empty tree.

***********************************************************************************************/
bool ONDDAS::ResetTree(int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return false;
	try
	{
		bistree_destroy(m_treeList[treeID].tree);
		delete m_treeList[treeID].tree;
		m_treeList[treeID].tree=NULL;
		m_treeList[treeID].tree=new BisTree;
		if (m_treeList[treeID].tree==NULL)
			return false;
		bistree_init(m_treeList[treeID].tree,ONDDASCompare,ONDDASDestroyElmt);
	}
	catch (...)
	{
		throw;
	}
	return true;
}
/***********************************************************************************************

  Returns the ID method of the specified data type ID. Returns -1 if it fails.
  Methods: ONDDAS_METHOD_NAME, ONDDAS_METHOD_NUMBER

***********************************************************************************************/
int ONDDAS::GetDataTypeMethod(int dataTypeID)
{
	if (dataTypeID<0 || dataTypeID>=m_dataTypeCount)
		return -1;
	return m_dataTypeList[dataTypeID].method;
}
/***********************************************************************************************

  Returns the data type ID of the data held in tree referenced by the entered treeID.
  Returns -1 if it fails.

***********************************************************************************************/
int ONDDAS::GetTreeDataType(int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return -1;
	return m_treeList[treeID].dataTypeID;
}
/***********************************************************************************************

  Returns the treeID of first tree in the system with the specified data type ID.
  Returns -1 if no such tree is found.

***********************************************************************************************/
int ONDDAS::FindTreeID(int dataTypeID)
{
	for (int i=0; i<m_treeCount; i++)
		if (m_treeList[i].dataTypeID==dataTypeID)
			return i;
	return -1;
}
/***********************************************************************************************

  Attempts to add data to tree.
  Returns false if it fails.

***********************************************************************************************/
bool ONDDAS::LoadInData(int id,int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return false;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NUMBER)
		return false;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].loadData.loadDataI==NULL)
		return false;
	void *data = m_dataTypeList[m_treeList[treeID].dataTypeID].loadData.loadDataI(id);
	if (data==NULL)
		return false;
	if (m_treeList[treeID].tree==NULL)
		return false;
	try
	{
		ONDDAS_Elmt *newElmt = NULL;
		newElmt = (ONDDAS_Elmt *)malloc(sizeof(ONDDAS_Elmt));
		if (newElmt==NULL)
			return false;
		newElmt->data		=data;
		newElmt->destroyData=m_dataTypeList[m_treeList[treeID].dataTypeID].destroyData;
		newElmt->id.number	=id;
		newElmt->method		=ONDDAS_METHOD_NUMBER;
		bistree_insert(m_treeList[treeID].tree,newElmt);
	}
	catch (...)
	{
		throw;
	}
	return true;
}
/***********************************************************************************************

  Attempts to add data to tree.
  Returns false if it fails.

***********************************************************************************************/
bool ONDDAS::LoadInData(const char *id,int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return false;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NAME)
		return false;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].loadData.loadDataC==NULL)
		return false;
	void *data = m_dataTypeList[m_treeList[treeID].dataTypeID].loadData.loadDataC((char *)id);
	if (data==NULL)
		return false;
	if (m_treeList[treeID].tree==NULL)
		return false;
	try
	{
		ONDDAS_Elmt *newElmt = NULL;
		newElmt = (ONDDAS_Elmt *)malloc(sizeof(ONDDAS_Elmt));
		if (newElmt==NULL)
			return false;
		char *newString = NULL;
		newString = (char *)malloc(strlen(id)+1);
		if (newString==NULL)
			return false;
		strcpy(newString,id);
		newElmt->data		=data;
		newElmt->destroyData=m_dataTypeList[m_treeList[treeID].dataTypeID].destroyData;
		newElmt->id.name	=newString;
		newElmt->method		=ONDDAS_METHOD_NAME;
		bistree_insert(m_treeList[treeID].tree,newElmt);
	}
	catch (...)
	{
		throw;
	}
	return true;
}
/***********************************************************************************************

  Attempts to insert data already loaded in ram, into tree.
  Returns false if it fails.

***********************************************************************************************/
bool ONDDAS::InsertData(int id,int treeID,void *data)
{
	if (treeID<0 || treeID>=m_treeCount)
		return false;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NUMBER)
		return false;
	if (data==NULL)
		return false;
	if (m_treeList[treeID].tree==NULL)
		return false;
	try
	{
		ONDDAS_Elmt *newElmt = NULL;
		newElmt = (ONDDAS_Elmt *)malloc(sizeof(ONDDAS_Elmt));
		if (newElmt==NULL)
			return false;
		newElmt->data		=data;
		newElmt->destroyData=m_dataTypeList[m_treeList[treeID].dataTypeID].destroyData;
		newElmt->id.number	=id;
		newElmt->method		=ONDDAS_METHOD_NUMBER;
		bistree_insert(m_treeList[treeID].tree,newElmt);
	}
	catch (...)
	{
		throw;
	}
	return true;
}
/***********************************************************************************************

  Attempts to insert data already loaded in ram, into tree.
  Returns false if it fails.

***********************************************************************************************/
bool ONDDAS::InsertData(const char *id,int treeID,void *data)
{
	if (treeID<0 || treeID>=m_treeCount || id==NULL)
		return false;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NAME)
		return false;
	if (data==NULL)
		return false;
	if (m_treeList[treeID].tree==NULL)
		return false;
	try
	{
		ONDDAS_Elmt *newElmt = NULL;
		newElmt = (ONDDAS_Elmt *)malloc(sizeof(ONDDAS_Elmt));
		if (newElmt==NULL)
			return false;
		char *newString = NULL;
		newString = (char *)malloc(strlen(id)+1);
		if (newString==NULL)
			return false;
		strcpy(newString,id);
		newElmt->data		=data;
		newElmt->destroyData=m_dataTypeList[m_treeList[treeID].dataTypeID].destroyData;
		newElmt->id.name	=newString;
		newElmt->method		=ONDDAS_METHOD_NAME;
		bistree_insert(m_treeList[treeID].tree,newElmt);
	}
	catch (...)
	{
		throw;
	}
	return true;
}
/***********************************************************************************************

  Returns a pointer to the data requested.
  Returns NULL if data could not be found or loaded into the system.

***********************************************************************************************/
void *ONDDAS::AccessData(int id, int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return NULL;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NUMBER)
		return NULL;
	void *result = FindData(id,treeID);
	if (result==NULL)
		if (LoadInData(id,treeID))
			result = FindData(id,treeID);
	return result;
}
/***********************************************************************************************

  Returns a pointer to the data requested.
  Returns NULL if data could not be found or loaded into the system.

***********************************************************************************************/
void *ONDDAS::AccessData(const char *id,int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return NULL;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NAME)
		return NULL;
	void *result = FindData(id,treeID);
	if (result==NULL)
		if (LoadInData(id,treeID))
			result = FindData(id,treeID);
	return result;
}
/***********************************************************************************************

  Sets highestID to the ID within the tree with the highest value.

***********************************************************************************************/
void ONDDAS::GetHighestID(int treeID, int &highestID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NUMBER)
		return;
	if (m_treeList[treeID].tree==NULL)
		return;
	BiTreeNode *node=bitree_root(m_treeList[treeID].tree);
	while (!bitree_is_eob(bitree_right(node)))
		node=bitree_right(node);
	highestID = ((ONDDAS_Elmt *)(((AvlNode *)(node)->data)->data))->id.number;
}
/***********************************************************************************************

  Sets highestID to the ID within the tree with the highest value.
  WARNING: Do not alter the data pointed to by highestID.

***********************************************************************************************/
void ONDDAS::GetHighestID(int treeID, char **highestID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NAME)
		return;
	if (m_treeList[treeID].tree==NULL)
		return;
	BiTreeNode *node=bitree_root(m_treeList[treeID].tree);
	while (!bitree_is_eob(bitree_right(node)))
		node=bitree_right(node);
	(*highestID) = ((ONDDAS_Elmt *)(((AvlNode *)(node)->data)->data))->id.name;
}
/***********************************************************************************************

  Sets lowestID to the ID within the tree with the lowest value.

***********************************************************************************************/
void ONDDAS::GetLowestID(int treeID, int &lowestID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NUMBER)
		return;
	if (m_treeList[treeID].tree==NULL)
		return;
	BiTreeNode *node=bitree_root(m_treeList[treeID].tree);
	while (!bitree_is_eob(bitree_left(node)))
		node=bitree_left(node);
	lowestID = ((ONDDAS_Elmt *)(((AvlNode *)(node)->data)->data))->id.number;
}
/***********************************************************************************************

  Sets lowestID to the ID within the tree with the lowest value.
  WARNING: Do not alter the data pointed to by lowestID.

***********************************************************************************************/
void ONDDAS::GetLowestID(int treeID, char **lowestID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NAME)
		return;
	if (m_treeList[treeID].tree==NULL)
		return;
	BiTreeNode *node=bitree_root(m_treeList[treeID].tree);
	while (!bitree_is_eob(bitree_left(node)))
		node=bitree_left(node);
	(*lowestID) = ((ONDDAS_Elmt *)(((AvlNode *)(node)->data)->data))->id.name;
}
/***********************************************************************************************

  Returns a pointer to the data requested.
  Returns NULL if data could not be found in system.

***********************************************************************************************/
void *ONDDAS::FindData(int id, int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return NULL;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NUMBER)
		return NULL;
	if (m_treeList[treeID].tree==NULL)
		return NULL;
	ONDDAS_Elmt searchElmt;
	searchElmt.data		  =NULL;
	searchElmt.destroyData=m_dataTypeList[m_treeList[treeID].dataTypeID].destroyData;
	searchElmt.id.number  =id;
	searchElmt.method	  =ONDDAS_METHOD_NUMBER;
	void *retval = (void *)&searchElmt;
	if (bistree_lookup(m_treeList[treeID].tree,&retval)==0)
		return ((ONDDAS_Elmt *)retval)->data;
	return NULL;
}
/***********************************************************************************************

  Returns a pointer to the data requested.
  Returns NULL if data could not be found in system.

***********************************************************************************************/
void *ONDDAS::FindData(const char *id,int treeID)
{
	if (treeID<0 || treeID>=m_treeCount)
		return NULL;
	if (m_dataTypeList[m_treeList[treeID].dataTypeID].method!=ONDDAS_METHOD_NAME)
		return NULL;
	if (m_treeList[treeID].tree==NULL)
		return NULL;
	ONDDAS_Elmt searchElmt;
	searchElmt.data		  =NULL;
	searchElmt.destroyData=m_dataTypeList[m_treeList[treeID].dataTypeID].destroyData;
	searchElmt.id.name	  =(char*)id;
	searchElmt.method	  =ONDDAS_METHOD_NAME;
	void *retval = (void *)&searchElmt;
	if (bistree_lookup(m_treeList[treeID].tree,&retval)==0)
		return ((ONDDAS_Elmt *)retval)->data;
	return NULL;
}
/***********************************************************************************************

  (Utility) sets an integer within system to be used as a constant for reliable access to
  the data stored in system.  After being set, the constant will not be reset, until the
  system is reset.

***********************************************************************************************/
void ONDDAS::SetConstant(const int c)
{
	if (m_firstTime)
		m_constant=c;
	m_firstTime=false;
}
/***********************************************************************************************

  (Utility) returns the value set by SetConstant.

***********************************************************************************************/
int ONDDAS::GetConstant()
{
	return m_constant;
}
/***********************************************************************************************

  Compares two ONDDAS elements and returns the id value relationship.

***********************************************************************************************/
int ONDDASCompare(const void *key1,const void *key2)
{
	int val1=NULL, val2=NULL;
	ONDDAS_Elmt *elmt1=(ONDDAS_Elmt *)key1, *elmt2=(ONDDAS_Elmt *)key2;
	if (ONDDAS_METHOD_NUMBER==elmt1->method)
	{
		val1=elmt1->id.number;
		val2=elmt2->id.number;
	}
	else
		val1=strcmp(elmt1->id.name,elmt2->id.name);
	if (val1>val2)
		return 1;
	else if (val1<val2)
		return -1;
	else
		return 0;
	return 0;
}
/***********************************************************************************************

  Destroys ONDDAS elements and calls the specific data type destructor.

***********************************************************************************************/
void ONDDASDestroyElmt(void *data)
{
	ONDDAS_Elmt *element=(ONDDAS_Elmt *)data;
	try
	{
		if (element->destroyData!=NULL)
			element->destroyData(element->data);
		element->data=NULL;
		if (ONDDAS_METHOD_NUMBER==element->method)
			free(element);
		else
		{
			free(element->id.name);
			free(element);
		}
	}
	catch (...)
	{
		throw;
	}
}
/***********************************************************************************************

  Use this function as the load function for data that should not be auto loaded by
  the system on acces request. ONDDASNullLoadDataI for data with an integer id.

***********************************************************************************************/
void *ONDDASNullLoadDataI(int id)
{
	return NULL;
}
/***********************************************************************************************

  Use this function as the load function for data that should not be auto loaded by
  the system on acces request. ONDDASNullLoadDataC for data with a char* id.

***********************************************************************************************/
void *ONDDASNullLoadDataC(char*id)
{
	return NULL;
}