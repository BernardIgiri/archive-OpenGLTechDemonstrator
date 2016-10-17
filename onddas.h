#ifndef __INCLUDED_ONDEMANDDATAACCESSSYSTEM__
#define __INCLUDED_ONDEMANDDATAACCESSSYSTEM__

#include "bistree.h"
#define ONDDAS_METHOD_NUMBER 0
#define ONDDAS_METHOD_NAME	 1

typedef struct
{
	union {
		int number;
		char *name;
	} id;
	int method;
	void (*destroyData)(void *data);
	void *data;
} ONDDAS_Elmt;

typedef struct
{
	int dataTypeID;
	BisTree *tree;
} ONDDAS_Tree;

typedef struct
{
	union {
		void *(*loadDataI)(int ID); // should load data after receiving data ID #, & allocate needed ram and return void pointer
		void *(*loadDataC)(char *ID); // should load data after receiving data ID string, & allocate needed ram and return void pointer
	} loadData; // set loadDataI for access by ID number and set loadDataC for access by an ID string
	int method; // comparison method
	void (*destroyData)(void *data); // should deallocate allocated ram // NULL if data is not dynamic
} ONDDAS_DataType;

class ONDDAS
{
public:
	ONDDAS();
	~ONDDAS();
	bool Initialize();
	void *AccessData(int id, int treeID);
	void *AccessData(const char *id, int treeID);
	bool InsertData(int id,int treeID, void *data);
	bool InsertData(const char *id,int treeID, void *data);
	bool LoadInData(int id,int treeID);
	bool LoadInData(const char *id,int treeID);
	int  RegisterDataType(void *(*loadData)(int ID),void (*destroyData)(void *data));
	int  RegisterDataType(void *(*loadData)(char *ID),void (*destroyData)(void *data));
	int  StartNewTree(int dataTypeID);
	int	 GetDataTypeMethod(int dataTypeID);
	int  GetTreeDataType(int treeID);
	int	 FindTreeID(int dataTypeID);
	void SetConstant(const int c);
	int	 GetConstant();
	void GetLowestID(int treeID, int &lowestID);
	void GetLowestID(int treeID, char **lowestID);
	void GetHighestID(int treeID, int &highestID);
	void GetHighestID(int treeID, char **highestID);
	bool ResetTree(int treeID);
	void Shutdown();
protected:
	void *FindData(int id, int treeID);
	void *FindData(const char *id, int treeID);
	int  m_treeCount,
		 m_dataTypeCount,
		 m_constant;
	bool m_firstTime;
	ONDDAS_Tree		*m_treeList;
	ONDDAS_DataType *m_dataTypeList;
};

int	 ONDDASCompare(const void *key1, const void *key2);
void ONDDASDestroyElmt (void *data);
void *ONDDASNullLoadDataI(int id);
void *ONDDASNullLoadDataC(char*id);
#endif